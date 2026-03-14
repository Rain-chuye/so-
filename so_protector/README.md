# Android Root SO Protector (Standalone Edition)

该工具专为 Android 平台下的 `.so` 动态库提供 root 环境下的安全加固。

## 核心功能
1. **指令级虚拟化 (VMP)**: 将 JNI 函数逻辑转换为字节码。
2. **全方位混淆**: 集成了 CFF (控制流扁平化) 和 MBA (混合布尔-算术) 变换。
3. **Android 特化防护**:
   - 检测 Frida、Xposed 等 Hook 框架。
   - 检测调试器状态。
   - 使用 memfd_create 实现无文件加载，对抗内存 Dump。

## 使用说明 (Android Root)
1. **交叉编译工具**: 使用 NDK 将 protector.cpp 编译为 Android 可执行文件。
2. **在 Android 上运行**: 将 protector 和目标 SO 推送到 /data/local/tmp/，使用 root 权限执行 protector。
3. **部署保护后的 SO**: 生成的 .protected 文件需要配合加载器使用。

## 技术原理
工具会在 SO 文件头部注入一个 VmpHeader，并对原始数据进行 MBA 变换加密。加载器在运行时通过 memfd_create 在匿名内存中解密并加载库文件，确保磁盘上不存在明文 SO。
