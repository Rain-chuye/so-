#include <iostream>
#include <vector>
#include <sys/mman.h>
#include <unistd.h>
#include <cstring>
#include <dlfcn.h>
#include <cstdint>

// MBA Transform
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

void mba_inverse(uint8_t* b, size_t s, uint8_t k) {
    for (size_t i = 0; i < s; ++i) {
        uint8_t ki = k ^ (i & 0xFF);
        uint8_t res = b[i];
        res = (res | 0x1F) - (res & 0x1F);
        res = (res ^ 0xAA) - 0x42;
        res = (res | ki) - (res & ki);
        b[i] = res;
    }
}

// Target function
void secret_function() {
    std::cout << ">>> [成功] 该代码是在运行时通过 MBA 动态解密的! <<<" << std::endl;
}

int main() {
    uint8_t key = 0xCC;
    size_t func_size = 64;

    std::cout << "--- SO 保护器原理演示 (MBA + 动态解密) ---" << std::endl;

    // 1. Initial State
    std::cout << "1. 原始函数调用:" << std::endl;
    secret_function();

    // 2. Encryption
    std::cout << "\n2. 正在使用 MBA 算法加密函数段..." << std::endl;
    uintptr_t page_start = (uintptr_t)secret_function & ~(sysconf(_SC_PAGESIZE) - 1);
    if (mprotect((void*)page_start, sysconf(_SC_PAGESIZE), PROT_READ | PROT_WRITE | PROT_EXEC) != 0) {
        perror("mprotect");
        return 1;
    }
    mba_transform((uint8_t*)secret_function, func_size, key);

    std::cout << "函数现已加密 (内存中的代码已变成 MBA 混淆后的数据)。" << std::endl;

    // 3. Dynamic Decryption
    std::cout << "\n3. 模拟 Stub 正在进行动态解密..." << std::endl;
    mba_inverse((uint8_t*)secret_function, func_size, key);

    std::cout << "4. 调用解密后的函数:" << std::endl;
    secret_function();

    std::cout << "\n--- 演示完成 ---" << std::endl;
    return 0;
}
