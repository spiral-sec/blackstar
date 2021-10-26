
#include "blackstar.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

char *gen_new_key(time_t seed)
{
    char *result = malloc(sizeof(char) * 129);

    srand(seed);
    if (!result)
        return NULL;
    for (int i = 0; i < 128; i++)
        result[i] = rand() % 256;
    result[128] = 0;
    return result;
}

int bl_decrypt_section(blackstar_t *bstar, const char *code_sname,
const char *key_sname, const char *bool_sname, crypter_t crypter)
{
    Elf64_Shdr *code_section = bl_find_section(bstar->content, code_sname);
    Elf64_Shdr *bool_section = bl_find_section(bstar->content, bool_sname);
    Elf64_Shdr *key_section = bl_find_section(bstar->content, key_sname);
    size_t code_len = 0;
    unsigned char *ptr = NULL;
    char *key = NULL;

    if (!code_section || !bool_section || !key_section)
        return 1;

    // Decrypting key section with code section
    // then code section with key section
    bl_naive_crypter(bstar, key_sname, code_sname, crypter);
    bl_naive_crypter(bstar, code_sname, key_sname, crypter);

    // gen_new_key here
    key = gen_new_key(time(NULL) * ((intptr_t)key_section % 255));
    ptr = bstar->content + key_section->sh_offset;
    bl_edit_section(ptr, key_section->sh_size, key);

    // Setting is_encrypted boolean to true
    ptr = bstar->content + bool_section->sh_offset;
    bl_change_section_wperm(ptr, bool_section->sh_size, true);
    for (size_t i = 0; i < bool_section->sh_size; i++)
        ptr[i] = 0;
    bl_change_section_wperm(ptr, bool_section->sh_size, false);

    // syncronising the binary
    bl_sync(bstar);
    return 0;
}

