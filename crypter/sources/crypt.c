
#include "crypter.h"
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>
#include <sys/mman.h>
#include <elf.h>

void *get_memory_map(crypter_t c)
{
    struct stat st;
    void *result = NULL;

    c.fd = open(c.target_binary, O_RDWR | O_APPEND, 0);
    if (c.fd < 0 || stat(c.target_binary, &st) < 0)
        return NULL;

    result = mmap(0, st.st_size, PROT_READ | PROT_WRITE | PROT_EXEC,
    MAP_SHARED, c.fd, 0);
    return result;
}

void __xor(unsigned char *mem, char *key)
{
    int size = strlen((char *)mem);
    int key_len = strlen(key);

    for (int i = 0; i < size; i++)
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


