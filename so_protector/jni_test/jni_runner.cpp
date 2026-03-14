#include <iostream>
#include <dlfcn.h>

typedef void* (*stringFromJNI_t)(void*, void*);
typedef int (*addNumbers_t)(void*, void*, int, int);

int main(int argc, char** argv) {
    if (argc < 2) return 1;
    void* h = dlopen(argv[1], RTLD_NOW);
    if (!h) { std::cerr << "dlopen fail: " << dlerror() << std::endl; return 1; }

    stringFromJNI_t stringFromJNI = (stringFromJNI_t)dlsym(h, "Java_com_example_myapp_MainActivity_stringFromJNI");
    addNumbers_t addNumbers = (addNumbers_t)dlsym(h, "Java_com_example_myapp_MainActivity_addNumbers");

    if (stringFromJNI) {
        std::cout << "JNI String: " << (const char*)stringFromJNI(nullptr, nullptr) << std::endl;
    }
    if (addNumbers) {
        std::cout << "JNI Add (10 + 20): " << addNumbers(nullptr, nullptr, 10, 20) << std::endl;
    }

    dlclose(h);
    return 0;
}
