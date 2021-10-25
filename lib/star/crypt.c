
#include "blackstar.h"

#include <string.h>
#include <unistd.h>
#include <sys/mman.h>

void xor_crypt(unsigned char *data, size_t data_len, char *key)
{
    int key_len = strlen(key);

    for (size_t i = 0; i < data_len; i++)
        data[i] ^= key[i % key_len];
}

int bl_change_section_wperm(unsigned char *ptr, size_t s_len, bool should_write)
{
    unsigned char *end_section = ptr + s_len;
    size_t p_size = sysconf(_SC_PAGESIZE);
    uintptr_t p_start = (uintptr_t)ptr & -p_size;
    unsigned int p_start_size = end_section - (unsigned char *)p_start;
    int perm_flags = (should_write) ?
    PROT_READ | PROT_WRITE | PROT_EXEC : PROT_READ | PROT_EXEC;

    if (mprotect((void *)p_start, p_start_size, perm_flags) < 0)
        return 1;
    return 0;
}

void bl_edit_section(unsigned char *ptr, size_t size, char *new_val)
{
    bl_change_section_wperm(ptr, size, true);
    for (size_t i = 0; i < size; i++)
        ptr[i] = new_val[i];
    bl_change_section_wperm(ptr, size, false);
}

int bl_encrypt_section(blackstar_t *bstar, const char *code_sname,
const char *key_sname, const char *bool_sname, crypter_t crypter, char *key)
{
    Elf64_Shdr *code_section = bl_find_section(bstar, code_sname);
    Elf64_Shdr *bool_section = bl_find_section(bstar, bool_sname);
    Elf64_Shdr *key_section = bl_find_section(bstar, key_sname);
    size_t code_len = 0;
    unsigned char *ptr = NULL;

    if (!code_section || !bool_section || !key_section)
        return 1;

    // Encrypting code section
    code_len = code_section->sh_size;
    ptr = bstar->content + code_section->sh_offset;
    bl_change_section_wperm(ptr, code_len, true);
    crypter(ptr, code_len, key);
    bl_change_section_wperm(ptr, code_len, false);

    // Setting key to key section
    ptr = bstar->content + key_section->sh_offset;
    bl_edit_section(ptr, key_section->sh_size, key);

    // Setting is_encrypted boolean to true
    ptr = bstar->content + bool_section->sh_offset;
    bl_change_section_wperm(ptr, bool_section->sh_size, true);
    for (size_t i = 0; i < bool_section->sh_size; i++)
        ptr[i] = 1;
    bl_change_section_wperm(ptr, bool_section->sh_size, false);

    // syncronising the binary
    bl_sync(bstar);
    return 0;
}

