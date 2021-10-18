
#include "elf_utils.h"
#include <elf.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

elf_utils_t *elf_read(char const *filepath)
{
    elf_utils_t *elf = malloc(sizeof(elf_utils_t));
    struct stat st = {0};
    int fd = 0;

    printf("[+] Attempting to read [%s]\n", filepath);
    fd = open(filepath, O_RDONLY);
    if (fd == -1 || stat(filepath, &st) < 0 || !elf)
        return NULL;
    elf->name = strdup(filepath);
    elf->content = malloc(sizeof(char) * (st.st_size + 1));
    printf("\t-== elf->content addr: %p  ==-\n", elf->content);
    if (read(fd, elf->content, st.st_size) < 0)
        return NULL;
    elf->content_len = st.st_size;
    elf->content[elf->content_len] = 0;
    printf("\t-== read %u bytes ==-\n", elf->content_len);
    close(fd);
    printf("[+] Successfully read the binary.\n");
    return elf;
}

void elf_destroy(elf_utils_t *elf)
{
    free(elf->content);
    free(elf->name);
    free(elf);
}

void elf_save_status(elf_utils_t *elf)
{
    int fd = 0;

    printf("[+] Saving ELF status...\n");
    if (unlink(elf->name) < 0)
        KILL("[!] Failed to delete binary while saving status");
    fd = open(elf->name, O_CREAT | O_TRUNC | O_RDWR, S_IRWXU);
    printf("\t-== file status: %d ==-\n", fd);
    if (fd == -1)
        KILL("[!] Failed to create a new file");
    if (write(fd, elf->content, elf->content_len) < 0)
        KILL("[!] Failed to write to a new file");
    close(fd);
}

static void elf_change_permissions(unsigned char *target_ptr,
unsigned int s_len, bool should_write)
{
    unsigned char *end_section = target_ptr + s_len;
    size_t p_size = sysconf(_SC_PAGESIZE);
    uintptr_t p_start = (uintptr_t)target_ptr & -p_size;
    unsigned int p_start_size = end_section - (unsigned char *)p_start;
    int perm_flags = (should_write) ? PROT_READ | PROT_WRITE | PROT_EXEC : PROT_READ | PROT_EXEC;

    printf("[+] Changing permissions for offset %p with length %u\n", target_ptr, s_len);
    printf("\t-== end_section:%p ==-\n", end_section);
    printf("\t-== p_size:%ld ==-\n", p_size);
    printf("\t-== p_start_size:%u ==-\n", p_start_size);
    if (mprotect((void *)p_start, p_start_size, perm_flags) < 0)
        KILL("[!] Could not change permissions for page");
    printf("[+] Done.\n\n");
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
            printf("[+] Found section %s at offset %ld\n",
            name, sym_header[i].sh_offset);
            return &sym_header[i];
        }
    }
    return NULL;
}

static void __xor(unsigned char *s_start, int s_len)
{
    for (int i = 0; i < s_len; i++)
        s_start[i] ^= (key[i] - 1);
}

static void generate_key(unsigned char *ptr, unsigned int size)
{
    for (unsigned int i = 0 ; i < size; i++) {
        ptr[i] = (rand() % 256);
        key[i] = ptr[i];
    }
}

void elf_decode(elf_utils_t *elf)
{
    Elf64_Shdr *code_section = elf_find_section(elf->content, ELF_CODE);

    printf("[+] Starting to decode binary...\n");
    if (!code_section)
        KILL("[!] Could not find Payload section");
    elf->s_offset = code_section->sh_offset;
    elf->s_len = code_section->sh_size;

    printf("\t-== Changing WRITE permissions to true ==-\n");
    elf_change_permissions((unsigned char *)&setup_payload, elf->s_len, true);

    printf("\t-== Decoding binary.==-\n");
    __xor((unsigned char *)&setup_payload, elf->s_len);
    __xor(elf->content + elf->s_offset, elf->s_len);

    printf("\t-== Changing WRITE permissions to false ==-\n");
    elf_change_permissions((unsigned char *)&setup_payload, elf->s_len, false);

    printf("\t-== Generating key... ==-\n");
    generate_key(elf->content + elf->s_offset, elf->s_len);

    __xor(elf->content + elf->s_offset, elf->s_len);
    elf_save_status(elf);
    printf("[+] Done decoding the binary.\n\n");
}


void generate_first_time_key(elf_utils_t *elf)
{
    Elf64_Shdr *code_section = elf_find_section(elf->content, ELF_CODE);
    unsigned char *boolean_pos = NULL;

    printf("[+] First time running [%s]. Attempting to build a basic Key.\n", elf->name);
    if (!code_section)
        KILL("[!] ???? No sections found ??? wtf bro ???");

    boolean_pos = elf->content + (int)code_section->sh_offset;
    printf("\t-== Setting ELF_KEY to false at addr %p ==-\n", boolean_pos);
    boolean_pos[0] = 0;
    boolean_pos[1] = 0;

    for (int i = 0; i < ELF_FUNC_SIZE; i++)
        key[i] = 1;
    printf("[+] Done.\n\n");
}

