# Android Root SO Protector (Portable Standalone Edition)

该工具专为 Android 平台下的 `.so` 动态库提供 Root 环境下的安全加固。

## 解决 "Not executable" 错误
如果你在 Android 上运行工具时报错 `not executable: 64-bit ELF file`，这是因为你使用的二进制文件架构（如 x86_64）与手机架构（如 ARM64）不匹配。

**解决方法：在目标手机上直接编译工具。**

## 安装与编译 (Android Termux)
1. 在手机上安装 **Termux**。
2. 安装编译器：
   ```bash
   pkg install clang
   ```
3. 编译工具：
   ```bash
   cd so_protector
   ./build_android.sh
   ```

## 核心功能
1. **指令级虚拟化 (VMP)**: 将 JNI 函数逻辑转换为私有字节码。
2. **全方位混淆**: 集成了 CFF (控制流扁平化) 和 MBA (混合布尔-算术) 变换。
3. **无文件加载**: 使用 `memfd_create` 实现内存解密加载，对抗内存 Dump。

## 使用说明 (Root 环境)
1. 将 `protector` 和目标 `SO` 推送到 `/data/local/tmp/`。
2. 使用 Root 权限执行：
   ```bash
   ./protector libnative.so
   ```
3. 生成的 `.protected` 文件需要配合 `android_stub.cpp` 编译的加载器使用。

## 技术原理
工具会对原始 SO 数据进行 MBA 变换加密，并注入 VMP 保护头。加载器在运行时通过 `memfd_create` 在匿名内存中解密并加载库文件，确保物理磁盘上不存在明文 SO。
