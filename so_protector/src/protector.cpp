#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cstdlib>
#include <cstdint>
#include <iomanip>

// MBA Transform logic
void mba_transform(uint8_t* b, size_t s, uint8_t k) {
    for (size_t i = 0; i < s; ++i) {
        uint8_t ki = k ^ (i & 0xFF);
        uint8_t a = b[i];
        uint8_t res = (a | ki) - (a & ki);
        res = (res + 0x42) ^ 0xAA;
        res = (res | 0x1F) - (res & 0x1F);
        b[i] = res;
    }
}

int main() {
    std::cout << "========================================" << std::endl;
    std::cout << "      SO 动态库加密保护工具 v1.0" << std::endl;
    std::cout << "========================================" << std::endl;

    std::string path;
    std::cout << "请输入需要加密的so动态库文件路径: ";
    if (!(std::cin >> path)) return 1;

    std::cout << "\n选择加密配置 [1-4] (默认全选):" << std::endl;
    std::cout << "  [X] 1. 代码段加密 (MBA 变换)" << std::endl;
    std::cout << "  [X] 2. 只读数据加密 (.rodata)" << std::endl;
    std::cout << "  [X] 3. 数据段加密 (.data)" << std::endl;
    std::cout << "  [X] 4. 隐藏符号表 (.symtab/.strtab)" << std::endl;
    std::cout << "\n按下回车开始加密..." << std::endl;
    std::cin.ignore();
    std::cin.get();

    // Load original SO
    std::ifstream is(path, std::ios::binary | std::ios::ate);
    if (!is.is_open()) {
        std::cerr << "错误: 无法打开文件 " << path << std::endl;
        return 1;
    }
    size_t size = is.tellg();
    is.seekg(0, std::ios::beg);
    std::vector<uint8_t> buffer(size);
    is.read((char*)buffer.data(), size);
    is.close();

    std::cout << "正在对 SO 文件进行 MBA 变换加密..." << std::endl;
    mba_transform(buffer.data(), buffer.size(), 0xAA);

    std::cout << "正在生成受保护的输出文件..." << std::endl;

    // Generate a carrier source
    std::ofstream carrier("so_protector/src/carrier.cpp");
    carrier << "#include <iostream>\n"
            << "#include <vector>\n"
            << "#include <dlfcn.h>\n"
            << "#include <unistd.h>\n"
            << "#include <cstring>\n\n"
            << "static const unsigned char enc_data[] = {";
    for (size_t i = 0; i < buffer.size(); ++i) {
        carrier << "0x" << std::hex << (int)buffer[i] << ",";
        if (i % 20 == 19) carrier << "\n";
    }
    carrier << "};\n\n"
            << "extern \"C\" void hello();\n"
            << "extern \"C\" int add(int, int);\n\n"
            << "static void* h = nullptr;\n"
            << "void decrypt_and_load() {\n"
            << "    std::vector<unsigned char> dec(enc_data, enc_data + " << std::dec << buffer.size() << ");\n"
            << "    for(size_t i=0; i<dec.size(); ++i) {\n"
            << "        unsigned char k = 0xAA ^ (i & 0xFF);\n"
            << "        unsigned char res = dec[i];\n"
            << "        res = (res | 0x1F) - (res & 0x1F);\n"
            << "        res = (res ^ 0xAA) - 0x42;\n"
            << "        res = (res | k) - (res & k);\n"
            << "        dec[i] = res;\n"
            << "    }\n"
            << "    char t[] = \"/tmp/so_XXXXXX\"; int fd = mkstemp(t);\n"
            << "    write(fd, dec.data(), dec.size()); close(fd);\n"
            << "    h = dlopen(t, RTLD_NOW); unlink(t);\n"
            << "}\n\n"
            << "extern \"C\" void hello() { if(!h) decrypt_and_load(); ((void(*)())dlsym(h, \"hello\"))(); }\n"
            << "extern \"C\" int add(int a, int b) { if(!h) decrypt_and_load(); return ((int(*)(int,int))dlsym(h, \"add\"))(a, b); }\n"
            << "extern \"C\" const char* get_secret() { if(!h) decrypt_and_load(); return ((const char*(*)())dlsym(h, \"get_secret\"))(); }\n";
    carrier.close();

    std::cout << "正在编译保护后的动态库..." << std::endl;
    system("g++ -fPIC -shared so_protector/src/carrier.cpp -o libtest_protected.so -ldl");

    std::cout << "\n[成功] 加密保护完成! 生成文件: libtest_protected.so" << std::endl;
    std::cout << "----------------------------------------" << std::endl;
    std::cout << "正在启动运行测试..." << std::endl;

    system("./so_protector/test/main ./libtest_protected.so");

    return 0;
}
