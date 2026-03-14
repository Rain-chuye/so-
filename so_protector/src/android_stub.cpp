#include <iostream>
#include <vector>
#include <sys/mman.h>
#include <unistd.h>
#include <dlfcn.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <cstring>
#include <sys/syscall.h>
#include <fstream>
#include <string>

struct VmpHeader {
    uint32_t magic;
    uint32_t version;
    uint32_t bytecode_offset;
    uint32_t bytecode_size;
};

static void* h = nullptr;

#ifndef SYS_memfd_create
#if defined(__arm__) || defined(__aarch64__)
#define SYS_memfd_create 279
#else
#define SYS_memfd_create 319
#endif
#endif

// Android Anti-Debug
static bool detect_frida() {
    std::ifstream maps("/proc/self/maps");
    std::string line;
    while (std::getline(maps, line)) {
        if (line.find("frida") != std::string::npos || line.find("agent.so") != std::string::npos) {
            return true;
        }
    }
    return false;
}

static void mba_inverse(uint8_t* b, size_t s, uint8_t k) {
    for (size_t i = 0; i < s; ++i) {
        uint8_t ki = k ^ (i & 0xFF);
        uint8_t res = b[i];
        res = (res | 0x1F) - (res & 0x1F);
        res = (res ^ 0xAA) - 0x42;
        res = (res | ki) - (res & ki);
        b[i] = res;
    }
}

extern "C" __attribute__((constructor)) void init_android_vmp() {
    if (detect_frida()) {
        std::cerr << "[Security] Frida detected! Security violation." << std::endl;
        _exit(1);
    }

    const char* path = "/data/local/tmp/libnative-lib.so.protected";
    int fd = open(path, O_RDONLY);
    if (fd < 0) return;

    VmpHeader header;
    if (read(fd, &header, sizeof(header)) != sizeof(header) || header.magic != 0x504d56) { close(fd); return; }

    std::vector<uint8_t> buffer(header.bytecode_size);
    if (read(fd, buffer.data(), header.bytecode_size) != (ssize_t)header.bytecode_size) { close(fd); return; }
    close(fd);

    mba_inverse(buffer.data(), buffer.size(), 0xBB);

    int mem_fd = syscall(SYS_memfd_create, "android_vmp_core", 0);
    if (mem_fd < 0) return;
    write(mem_fd, buffer.data(), buffer.size());

    char mem_path[64];
    sprintf(mem_path, "/proc/self/fd/%d", mem_fd);
    h = dlopen(mem_path, RTLD_NOW);
}

extern "C" {
    int Java_com_example_myapp_MainActivity_addNumbers(void* env, void* thiz, int a, int b) {
        if (!h) return 0;
        typedef int (*f_t)(void*, void*, int, int);
        auto f = (f_t)dlsym(h, "Java_com_example_myapp_MainActivity_addNumbers");
        return f ? f(env, thiz, a, b) : 0;
    }
}
