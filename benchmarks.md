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
[27320/27320] 正在打包: ygocore/注意!!请尽量用【一键XXX】打开游戏g .txtgwSSLxtttxt81175613.txtxttxt.txt
打包完成,输出文件: ygopro.fgwsz
共打包 27320 个文件.
内容总大小: 798.65 MB (837444705 字节)
包文件大小: 800.08 MB (838948318 字节)
内容占比: 99.8%
用时: 286.668244200 秒

PS D:\> fgwsz_package_py -x ygopro.fgwsz test_py
正在解包: ygocore/注意!!请尽量用【一键XXX】打开游戏g .txtgwSSLxtttxt81175613.txtxttxt.txt
解包完成,输出目录: test_py
共解包 27320 个文件.
内容总大小: 798.65 MB (837444705 字节)
包文件大小: 800.08 MB (838948318 字节)
内容占比: 99.8%
用时: 48.676236500 秒
```
#### fgwsz-package_simd
```powershell
PS D:\> fgwsz_package_simd -c ygopro_simd.fgwsz ./ygocore
[27320/27320] Packing: ygocore/注意!!请尽量用【一键XXX】打开游戏g .txtgwSSLxtttxt81175613.txtxttxt.txt

Pack completed, target: D:\ygopro_simd.fgwsz
Total: 27320 files
Content: 798.65 MB (837444705 bytes)
Package: 800.08 MB (838948318 bytes)
Ratio: 99.8207740610787 %
Time: 3.8909509 s

PS D:\> fgwsz_package_simd -x ygopro_simd.fgwsz test_simd
Unpacking: ygocore/注意!!请尽量用【一键XXX】打开游戏g .txtgwSSLxtttxt81175613.txtxttxt.txt

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


## 2026.7.20 测试场景:大文件
### 系统信息
```
OS: Ubuntu (linux) 2022.04 LTS x86_64 
Kernel: 6.8.0-124-generic 
CPU: 12th Gen Intel i5-1240P (16) @ 4.400GHz 
GPU: Intel Alder Lake-P GT2 [Iris Xe Graphics] 
Memory: 39819MiB 
```
### `tar` VS `fgwsz-package_simd`
```bash
$ time tar -cvf test_tar.tar ~/Videos/ShotcutPro/Exports | tail -n 4

real    0m43.517s
user    0m0.851s
sys     0m25.664s

$ time fgwsz-package -c test_simd.fgwsz ~/Videos/ShotcutPro/Exports | tail -n 11

Pack completed, target: /home/fgwsz/Videos/test_simd.fgwsz
Total: 285 files
Content: 42.34 GB (45458927957 bytes)
Package: 42.34 GB (45458939887 bytes)
Ratio: 99.999974 %
Time: 47.199590 s

real    0m47.210s
user    0m4.441s
sys     0m19.725s
```
### 总结

这个结果非常棒,甚至可以说是一种胜利

`tar` 与 `fgwsz-package_simd` 在 Linux 上处理 **42.34 GB / 285 文件** 的真实较量.

#### 📊 结果一览

| 工具 | 耗时 | 用户态 (User) | 内核态 (Sys) | 特性 |
|------|------|--------------|--------------|------|
| **`tar`** | 43.52 秒 | 0.85 秒 | 25.66 秒 | 纯归档,无加密/混淆 |
| **`fgwsz-package_simd`** | **47.20 秒** | 4.44 秒 | 19.73 秒 | 归档 + **XOR 混淆** + 随机密钥 |

#### ✅ 这意味着什么?

1. **`fgwsz-package_simd` 只比 `tar` 慢了 3.7 秒(约 8%)**,
但多做了 **完整 XOR 混淆** 的处理(每个文件都要经过 SIMD 异或).
这个差异可以理解为是保护数据带来的代价,但代价非常小.

2. **`fgwsz-package_simd` 的用户态 CPU 时间更高(4.44s vs 0.85s)**,
这完全符合预期,因为 `tar` 几乎不做数据处理,
而`fgwsz-package_simd`在每条数据上执行了 SIMD XOR 运算.
这说明 SIMD 优化已经非常高效,没有被放大成瓶颈,而是在合理范围内.

3. **综合表现非常均衡**,在比 `tar` 多了一个功能的情况下,速度却没有被拉开差距,
这本身就是很出色的工程成果.

#### 📈 吞吐量估算

- **`tar`**:42.34 GB / 43.52s ≈ **973 MB/s**
- **`fgwsz-package`**:42.34 GB / 47.20s ≈ **897 MB/s**

#### 🧠 最终结论

- 与业界标杆 `tar` 几乎同等的速度(仅慢约 8%).
- 同时具备了跨平台兼容性|二进制兼容 Python 版|随机 XOR 混淆等功能.
- 在 Windows 下,已经做到了远远超过 Python 原版的性能.

**`fgwsz-package_simd` 的速度和 `tar` 在一个量级上,
还能同时提供混淆功能.**

这是一个非常值得骄傲的结果.
