
#include "crypter.h"
#include <fcntl.h>
#include <stddef.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>
#include <sys/mman.h>
#include <stdlib.h>
#include <elf.h>

static unsigned char *get_memory_map(crypter_t c, size_t *size)
{
    struct stat st;
    unsigned char *result = NULL;

    if (c.fd < 0 || stat(c.target_binary, &st) < 0)
        return NULL;
    *size = st.st_size;
    result = malloc(sizeof(unsigned char) * (st.st_size + 1));
    if (!result || read(c.fd, result, st.st_size) < 0)
        return NULL;
    return result;
}

void __xor(unsigned char *mem, size_t size, char *key)
{
    int key_len = strlen(key);

    for (unsigned int i = 0; i < size; i++)
        mem[i] ^= key[i % key_len];
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

static void xor_section(void *data, Elf64_Shdr *shdr, char *key)
{
    unsigned char *ch = (unsigned char *)data;
    int offset = shdr->sh_offset;
    int size = shdr->sh_size;

    ch += offset;
    __xor(ch, size, key);
}

static void set_section_key(unsigned char *data, char *key, char *key_sname)
{
    Elf64_Shdr *shdr = elf_find_section(data, key_sname);
    unsigned char *ch = data + shdr->sh_offset;

    for (unsigned int i = 0; i < shdr->sh_size; i++)
        ch[i] = key[i];
}

static void save_elf_status(unsigned char *data, char *bin_name, size_t size)
{
    int fd = 0;

    if (unlink(bin_name) < 0)
        return;
    fd = open(bin_name, O_CREAT | O_TRUNC | O_RDWR, S_IRWXU);
    if (fd == -1)
        return;
    if (write(fd, data, size) < 0)
        return;
}

void encrypt(crypter_t c)
{
    unsigned char *data = NULL;
    Elf64_Shdr *shdr = NULL;
    size_t size = 0;

    if (c.fd == -1)
        return;
    data = get_memory_map(c, &size);
    shdr = elf_find_section(data, c.target_section_name);
    xor_section(data, shdr, c.key);

    // once this is correctly set, we will need to set the boolean thingy to true
    set_section_key(data, c.key, c.key_section_name);
    close(c.fd);
    save_elf_status(data, c.target_binary, size);
}
