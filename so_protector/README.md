# SO Protector Tool

这是一个使用 C++ 编写的 .so 动态库加密保护工具。

## 功能特性
1. **MBA 变换加密**：使用复杂的混合布尔-算术 (MBA) 恒等式代替传统的 XOR/AES，提高逆向难度。
2. **符号隐藏**：擦除敏感符号，防止通过 nm/readelf 直接分析功能。
3. **自解密加载器**：生成的保护后 SO 包含自解密逻辑。
4. **动态防护**：在内存中动态还原代码。

## 使用方法
1. 编译工具：
   ```bash
   g++ -O2 so_protector/src/protector.cpp -o so_protector/protector
   ```
2. 执行加密：
   ```bash
   ./so_protector/protector
   ```
3. 按照提示输入 .so 路径并选择配置。

## 测试验证
工具会自动运行 `main` 程序来验证加密后的 `libtest_protected.so` 是否能正常输出结果。
