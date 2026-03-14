#include <sys/mman.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <ucontext.h>
#include <sys/ptrace.h>
#include <vector>

// This stub will be compiled separately and injected.
// It needs to be self-contained or use standard syscalls.

extern "C" {

struct ProtectedRange {
    unsigned long start;
    unsigned long size;
    unsigned char key;
    int is_encrypted; // 1 if encrypted, 0 if decrypted
};

// These will be filled by the protector tool
ProtectedRange ranges[10];
int num_ranges = 0;
unsigned long last_decrypted_page = 0;
size_t last_decrypted_size = 0;
unsigned char last_key = 0;

void mba_inverse_transform(unsigned char* buffer, size_t size, unsigned char key) {
    for (size_t i = 0; i < size; ++i) {
        unsigned char k = key ^ (i & 0xFF);
        unsigned char res = buffer[i];
        res = (res | 0x1F) - (res & 0x1F);
        res = (res ^ 0xAA) - 0x42;
        res = (res | k) - (res & k);
        buffer[i] = res;
    }
}

void mba_transform(unsigned char* buffer, size_t size, unsigned char key) {
    for (size_t i = 0; i < size; ++i) {
        unsigned char k = key ^ (i & 0xFF);
        unsigned char a = buffer[i];
        unsigned char res = (a | k) - (a & k);
        res = (res + 0x42) ^ 0xAA;
        res = (res | 0x1F) - (res & 0x1F);
        buffer[i] = res;
    }
}

void anti_debug() {
    if (ptrace(PTRACE_TRACEME, 0, 1, 0) < 0) {
        // Already being traced
        _exit(1);
    }
    ptrace(PTRACE_DETACH, 0, 1, 0);
}

void segv_handler(int sig, siginfo_t* si, void* unused) {
    unsigned long addr = (unsigned long)si->si_addr;

    // Find which range this address belongs to
    for (int i = 0; i < num_ranges; ++i) {
        if (addr >= ranges[i].start && addr < ranges[i].start + ranges[i].size) {

            // Re-encrypt previous page if it exists
            if (last_decrypted_page) {
                mprotect((void*)last_decrypted_page, last_decrypted_size, PROT_READ | PROT_WRITE);
                mba_transform((unsigned char*)last_decrypted_page, last_decrypted_size, last_key);
                mprotect((void*)last_decrypted_page, last_decrypted_size, PROT_NONE);
            }

            // Decrypt current page
            unsigned long page_start = addr & ~(sysconf(_SC_PAGESIZE) - 1);
            size_t page_size = sysconf(_SC_PAGESIZE);

            // Ensure we stay within range
            if (page_start < ranges[i].start) page_start = ranges[i].start;
            if (page_start + page_size > ranges[i].start + ranges[i].size) {
                page_size = ranges[i].start + ranges[i].size - page_start;
            }

            mprotect((void*)page_start, page_size, PROT_READ | PROT_WRITE);
            mba_inverse_transform((unsigned char*)page_start, page_size, ranges[i].key);

            // Set permissions back (allow execute if it's code)
            // For simplicity, we use READ|WRITE|EXEC here, but in real life we should distinguish
            mprotect((void*)page_start, page_size, PROT_READ | PROT_WRITE | PROT_EXEC);

            last_decrypted_page = page_start;
            last_decrypted_size = page_size;
            last_key = ranges[i].key;

            return;
        }
    }

    // If not in our ranges, crash properly
    signal(SIGSEGV, SIG_DFL);
}

__attribute__((constructor))
void init_protector() {
    anti_debug();

    struct sigaction sa;
    sa.sa_flags = SA_SIGINFO;
    sigemptyset(&sa.sa_mask);
    sa.sa_sigaction = segv_handler;
    sigaction(SIGSEGV, &sa, NULL);

    // Initial state: all protected segments are PROT_NONE
    for (int i = 0; i < num_ranges; ++i) {
        mprotect((void*)ranges[i].start, ranges[i].size, PROT_NONE);
    }
}

}
