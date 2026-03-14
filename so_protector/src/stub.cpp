#include <sys/mman.h>
#include <signal.h>
#include <unistd.h>
#include <cstring>
#include <stdint.h>
#include <dlfcn.h>
#include <link.h>

extern "C" {

static uint8_t key = 0xAA;

static void handler(int sig, siginfo_t* si, void* unused) {
    uintptr_t addr = (uintptr_t)si->si_addr;
    uintptr_t ps = addr & ~(sysconf(_SC_PAGESIZE) - 1);
    size_t sz = sysconf(_SC_PAGESIZE);

    mprotect((void*)ps, sz, PROT_READ | PROT_WRITE | PROT_EXEC);
    for(size_t i=0; i<sz; ++i) {
        ((uint8_t*)ps)[i] ^= (key ^ (( (uintptr_t)ps + i) & 0xFF));
    }
}

__attribute__((constructor)) void init_protector() {
    struct sigaction sa;
    sa.sa_flags = SA_SIGINFO;
    sigemptyset(&sa.sa_mask);
    sa.sa_sigaction = handler;
    sigaction(SIGSEGV, &sa, NULL);

    // Protect just the 'hello' page for testing
    void* h = dlsym(RTLD_DEFAULT, "hello");
    if(h) {
        uintptr_t ps = (uintptr_t)h & ~(sysconf(_SC_PAGESIZE) - 1);
        mprotect((void*)ps, sysconf(_SC_PAGESIZE), PROT_NONE);
    }
}

}
