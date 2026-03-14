#include <iostream>
#include <dlfcn.h>
int main(int argc, char** argv) {
    void* h = dlopen(argv[1], RTLD_NOW);
    if(!h) { std::cerr << dlerror() << std::endl; return 1; }
    auto add = (int(*)(int,int))dlsym(h, "add");
    if(add) std::cout << "3 + 4 = " << add(3, 4) << std::endl;
    dlclose(h);
    return 0;
}
