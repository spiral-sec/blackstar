
#include "blackstar.h"

#include <elf.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>
#include <stdlib.h>

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

char *bl_get_section_content(blackstar_t *bstar, const char *key, size_t *size)
{
    Elf64_Shdr *shdr = bl_find_section(bstar->content, key);
    char *result = NULL;
    unsigned char *ptr = NULL;

    if (!shdr)
        return NULL;
    ptr = bstar->content + shdr->sh_offset;
    *size = shdr->sh_size;

    result = malloc(sizeof(char) * (*size + 1));
    if (!result)
        return NULL;

    for (size_t i = 0; i < *size; i++)
        result[i] = ptr[i];
    result[*size] = 0;
    return result;
}

void bl_naive_crypter(blackstar_t *bstar, const char *target_sname,
const char *key_sname, crypter_t cr)
{
    Elf64_Shdr *target = bl_find_section(bstar->content, target_sname);
    size_t key_size = 0;
    char *key = bl_get_section_content(bstar, key_sname, &key_size);
    unsigned char *ptr = NULL;
    size_t target_size = 0;

    if (!key || !target)
        return;
    ptr = bstar->content + target->sh_offset;
    target_size = target->sh_size;

    bl_change_section_wperm(ptr, target_size, true);
    cr(ptr, target_size, key, key_size);
    bl_change_section_wperm(ptr, target_size, false);

    free(key);
}

int bl_encrypt_section(blackstar_t *bstar, const char *code_sname,
const char *key_sname, const char *bool_sname, crypter_t crypter, char *key)
{
    Elf64_Shdr *code_section = bl_find_section(bstar->content, code_sname);
    Elf64_Shdr *bool_section = bl_find_section(bstar->content, bool_sname);
    Elf64_Shdr *key_section = bl_find_section(bstar->content, key_sname);
    size_t code_len = 0;
    unsigned char *ptr = NULL;

    if (!code_section || !bool_section || !key_section)
        return 1;

    // Setting key to key section
    ptr = bstar->content + key_section->sh_offset;
    bl_edit_section(ptr, key_section->sh_size, key);

    // Encrypting code section with key section,
    // then key section with code section
    bl_naive_crypter(bstar, code_sname, key_sname, crypter);
    bl_naive_crypter(bstar, key_sname, code_sname, crypter);

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

