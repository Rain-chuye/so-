#ifndef ELF_HELPER_HPP
#define ELF_HELPER_HPP

#include <elf.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <vector>
#include <string>
#include <iostream>
#include <cstring>

struct ElfFileInfo {
    void* data;
    size_t size;
    Elf64_Ehdr* ehdr;
    Elf64_Phdr* phdr;
    Elf64_Shdr* shdr;
    char* shstrtab;
};

class ElfHelper {
public:
    static bool load(const std::string& path, ElfFileInfo& info) {
        int fd = open(path.c_str(), O_RDWR);
        if (fd < 0) return false;

        struct stat st;
        fstat(fd, &st);
        info.size = st.st_size;
        info.data = mmap(NULL, info.size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
        close(fd);

        if (info.data == MAP_FAILED) return false;

        info.ehdr = (Elf64_Ehdr*)info.data;
        if (memcmp(info.ehdr->e_ident, ELFMAG, SELFMAG) != 0) {
            munmap(info.data, info.size);
            return false;
        }

        info.phdr = (Elf64_Phdr*)((uint8_t*)info.data + info.ehdr->e_phoff);
        info.shdr = (Elf64_Shdr*)((uint8_t*)info.data + info.ehdr->e_shoff);
        info.shstrtab = (char*)((uint8_t*)info.data + info.shdr[info.ehdr->e_shstrndx].sh_offset);

        return true;
    }

    static void unload(ElfFileInfo& info) {
        msync(info.data, info.size, MS_SYNC);
        munmap(info.data, info.size);
    }

    static Elf64_Shdr* find_section(ElfFileInfo& info, const std::string& name) {
        for (int i = 0; i < info.ehdr->e_shnum; ++i) {
            if (name == (info.shstrtab + info.shdr[i].sh_name)) {
                return &info.shdr[i];
            }
        }
        return nullptr;
    }
};

#endif
