
#include "crypter.h"
#include <fcntl.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>
#include <sys/mman.h>
#include <elf.h>

void *get_memory_map(crypter_t c)
{
    struct stat st;
    void *result = NULL;

    if (c.fd < 0 || stat(c.target_binary, &st) < 0)
        return NULL;

    result = mmap(0, st.st_size, PROT_READ | PROT_WRITE | PROT_EXEC,
    MAP_SHARED, c.fd, 0);
    return result;
}

void __xor(unsigned char *mem, size_t size, char *key)
{
    int key_len = strlen(key);

    for (unsigned int i = 0; i < size; i++)
        *(mem + i) ^= *(key + (i % key_len));
}

static Elf64_Shdr *elf_find_section(void *hdr, char const *name)
{
    char *_name = NULL;
    Elf64_Ehdr *elf_header = (Elf64_Ehdr *)hdr;
    Elf64_Shdr *sym_header = (Elf64_Shdr *)(hdr + elf_header->e_shoff);
    Elf64_Shdr *symb_table = &sym_header[elf_header->e_shstrndx];
    const char *s_tableptr = hdr + symb_table->sh_offset;

    for (int i = 0; i < elf_header->e_shnum; i++) {
        _name = (char *)(s_tableptr + sym_header[i].sh_name);
        if (!strcmp(_name, name)) {
            return &sym_header[i];
        }
    }
    return NULL;
}

void decrypt(unsigned char *ptr, unsigned char *ptr2, char *key)
{
    size_t pagesize = sysconf(_SC_PAGESIZE);
    uintptr_t pagestart = (uintptr_t)ptr & -pagesize;
    int psize = (ptr2 - (unsigned char *)ptr);

    if (mprotect((void *)pagestart, psize,
    PROT_READ | PROT_WRITE | PROT_EXEC) < 0)
        perror("mprotect:");
    __xor(ptr, ptr - ptr2, key);
}

static void xor_section(void *data, Elf64_Shdr *shdr, char *key)
{
    unsigned char *ch = (unsigned char *)data;
    int offset = shdr->sh_offset;
    int size = shdr->sh_size;

    ch += offset;
    __xor(ch, size, key);
}

void encrypt(crypter_t c)
{
    void *data = NULL;
    Elf64_Shdr *shdr = NULL;

    if (c.fd == -1)
        return;
    data = get_memory_map(c);
    shdr = elf_find_section(data, c.target_section_name);
    xor_section(data, shdr, c.key);
}
