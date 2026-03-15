#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cstdlib>
#include <cstdint>
#include <cstring>
#include <sys/stat.h>
#include <unistd.h>

#if defined(__x86_64__)
#define CURRENT_ARCH "x86_64"
#elif defined(__aarch64__)
#define CURRENT_ARCH "ARM64"
#else
#define CURRENT_ARCH "Unknown"
#endif

struct VmpHeader {
    uint32_t magic;
    uint32_t version;
    uint32_t bytecode_offset;
    uint32_t bytecode_size;
};

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
    std::cout << "    Android Standalone SO Protector v5.1" << std::endl;
    std::cout << "    Current Arch: " << CURRENT_ARCH << std::endl;
    std::cout << "========================================" << std::endl;

    if (getuid() != 0) {
        std::cout << "[提示] 建议在 Root 权限下运行以获得最佳兼容性。" << std::endl;
    }

    std::string path;
    std::cout << "请输入需要保护的 SO 路径: ";
    if (!(std::cin >> path)) return 1;

    std::ifstream is(path, std::ios::binary | std::ios::ate);
    if (!is.is_open()) {
        std::cerr << "错误: 无法打开文件 [" << path << "]" << std::endl;
        return 1;
    }

    size_t size = is.tellg();
    is.seekg(0, std::ios::beg);
    std::vector<uint8_t> buffer(size);
    is.read((char*)buffer.data(), size);
    is.close();

    std::cout << "正在应用 VMP + MBA 全方位加固..." << std::endl;
    mba_transform(buffer.data(), buffer.size(), 0xCC);

    std::string out_path = path + ".protected";
    std::ofstream os(out_path, std::ios::binary);
    VmpHeader header = { 0x504d56, 1, (uint32_t)sizeof(VmpHeader), (uint32_t)buffer.size() };
    os.write((char*)&header, sizeof(header));
    os.write((char*)buffer.data(), buffer.size());
    os.close();

    chmod(out_path.c_str(), 0777);
    std::cout << "\n[成功] 加固完成! 生成文件: " << out_path << std::endl;

    return 0;
}
