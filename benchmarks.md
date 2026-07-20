# 基准测试结果记录
## 2026.7.20 测试场景:大量小文件
### `Windows`系统
#### 系统信息
```
OS: Windows 11 家庭中文版 (25H2) x86_64
Kernel: WIN32_NT 10.0.26200.8655
CPU: Intel(R) Core(TM) i5-9400 (6) @ 4.10 GHz
GPU: Intel(R) UHD Graphics 630 (128.00 MiB) [Integrated]
Memory: 15.90 GiB
```
#### fgwsz-package_py
```powershell
PS D:\> fgwsz_package_py -c ygopro.fgwsz ./ygocore
[27320/27320] 正在打包: ygocore/注意！!请尽量用【一键XXX】打开游戏g .txtgwSSLxtttxt81175613.txtxttxt.txt
打包完成，输出文件: ygopro.fgwsz
共打包 27320 个文件。
内容总大小: 798.65 MB (837444705 字节)
包文件大小: 800.08 MB (838948318 字节)
内容占比: 99.8%
用时: 286.668244200 秒

PS D:\> fgwsz_package_py -x ygopro.fgwsz test_py
正在解包: ygocore/注意！!请尽量用【一键XXX】打开游戏g .txtgwSSLxtttxt81175613.txtxttxt.txt
解包完成，输出目录: test_py
共解包 27320 个文件。
内容总大小: 798.65 MB (837444705 字节)
包文件大小: 800.08 MB (838948318 字节)
内容占比: 99.8%
用时: 48.676236500 秒
```
#### fgwsz-package_simd
```powershell
PS D:\> fgwsz_package_simd -c ygopro_simd.fgwsz ./ygocore
[27320/27320] Packing: ygocore/注意！!请尽量用【一键XXX】打开游戏g .txtgwSSLxtttxt81175613.txtxttxt.txt

Pack completed, target: D:\ygopro_simd.fgwsz
Total: 27320 files
Content: 798.65 MB (837444705 bytes)
Package: 800.08 MB (838948318 bytes)
Ratio: 99.8207740610787 %
Time: 3.8909509 s

PS D:\> fgwsz_package_simd -x ygopro_simd.fgwsz test_simd
Unpacking: ygocore/注意！!请尽量用【一键XXX】打开游戏g .txtgwSSLxtttxt81175613.txtxttxt.txt

Unpack completed, target: D:\test_simd
Total: 27320 files
Content: 798.65 MB (837444705 bytes)
Package: 800.08 MB (838948318 bytes)
Ratio: 99.8207740610787 %
Time: 38.9753351 s
```
#### 数据对比

| 操作 | Python 原版 | C++ SIMD 版 (fgwsz-package_simd) | 提升倍数 |
|------|------------|--------------------------------|---------|
| 打包 27320 个文件 (800MB) | 286.67 秒 | **3.89 秒** | **≈73.7×** 🚀 |
| 解包 27320 个文件 (800MB) | 48.68 秒 | **38.98 秒** | **≈1.25×** ✅ |

