
#include "elf_utils.h"
#include <elf.h>
#include <stdint.h>
#include <sys/stat.h>

static Elf64_Shdr *find_section(void *ptr, const char *to_find)
{
    Elf64_Ehdr* elf_hdr = (Elf64_Ehdr *)ptr;
    Elf64_Shdr *shdr = (Elf64_Shdr *)(ptr + elf_hdr->e_shoff);
    Elf64_Shdr *sh_strtab = &shdr[elf_hdr->e_shstrndx];
    const char *const sh_strtab_ptr = ptr + sh_strtab->sh_offset;
    register int i = -1;

    while (++i < elf_hdr->e_shnum) {
        char *s_name = (char *)(sh_strtab_ptr + shdr[i].sh_name);
        if (s_name != NULL && !strcmp (s_name, to_find))
            return &shdr[i];
    }
    return NULL;
}

static void change_permissions(unsigned char *ptr, const int s_len, const bool should_write)
{
    unsigned char *end_of_section = ptr + s_len;
    size_t pagesize = sysconf(_SC_PAGESIZE);
    uintptr_t pagestart = (uintptr_t)ptr & -pagesize;
    register int psize = end_of_section - (unsigned char*)pagestart;

    if (should_write) {
        if (mprotect((void*)pagestart, psize, PROT_READ | PROT_WRITE | PROT_EXEC) < 0)
            KILL("FAILURE at allow_write_on_page");
    } else if (mprotect((void*)pagestart, psize, PROT_READ | PROT_EXEC) < 0)
        KILL("FAILURE at reset_page_perms");
}

static char *gen_key(unsigned char *ptr, const int size)
{
    register int ctr = -1;

    while (++ctr < size) {
        ptr[ctr] = rand() % 255;
        key[ctr] = ptr[ctr];
    }
    return key;
}

static void __xor(unsigned char *ptr, int size)
{
    register int ctr = -1;

    while (size > ++ctr)
        ptr[ctr] ^= (key[ctr] - 1);
}

static void save_elf_status(const elf_utils_t *utils)
{
    register int fd = 0;

    __log("Saving ELF status");
    if (unlink(utils->bin.name) < 0 ||
        (fd = open(utils->bin.name, O_CREAT | O_TRUNC | O_RDWR, S_IRWXU) < 0)) {
        KILL("save_elf_status");
    } else if (write(fd, utils->bin.content, utils->bin.len) < 0)
        KILL("save_elf_status (write)");
    close(fd);
    __log("Success");
}

static void gen_first_time_key(const elf_utils_t *utils)
{
    Elf64_Shdr *section = find_section(utils->bin.content, ELF_BOOL);
    unsigned char *bool_position = NULL;
    register int i = -1;

    if (!section)
        KILL("FAILURE at gen_first_time_key");
    __log("Running program for the first time");

    // Patching is_first_time to false in the binary itself
    bool_position = utils->bin.content + section->sh_offset;
    if (!bool_position)
        KILL("FAILURE looking for bool_position");
    bool_position[0] = 0;
    bool_position[1] = 0;
    __log("Set bool section to false");

    while (++i < ELF_FUNC_SIZE)
        key[i] = 0;
}

void elf_decode(elf_utils_t *utils)
{
    Elf64_Shdr *target_section = find_section(utils->bin.content, ELF_CODE);
    register int key_offset = -1;

    __log("Attempting to decode binary");
    if (is_first_time)
        gen_first_time_key(utils);

    if (!target_section)
        KILL("Could not code section");

    utils->s_offset = target_section->sh_offset;
    utils->s_len = target_section->sh_size;

    target_section = find_section(utils->bin.content, ELF_KEY);
    if (!target_section)
        KILL("Could not find key section");

    unsigned char *payload_addr = (unsigned char *)&target_section->sh_addr;

    key_offset = target_section->sh_offset;
    __log("Changing permissions and decoding data");
    change_permissions(payload_addr, utils->s_len, true);
    __xor(payload_addr, utils->s_len);
    __xor(utils->bin.content + utils->s_offset, utils->s_len);
    change_permissions(payload_addr, utils->s_len, false);

    gen_key(utils->bin.content + key_offset, utils->s_len);
    __xor(utils->bin.content + utils->s_offset, utils->s_len);
    save_elf_status(utils);
}

elf_utils_t *elf_read(char const *filepath)
{
    register int fd = 0;
    static bool has_been_read = false;
    static elf_utils_t utils = {0};
    struct stat s = {0};
    register int r_status = -1;

    if (has_been_read)
        return &utils;
    __log("Reading binary name");
    strncpy(utils.bin.name, filepath, ELF_NAMELEN);
    fd = open(utils.bin.name, O_RDONLY);
    if (fd == -1 || fstat(fd, &s) < 0) {
        KILL("Could not open binary");
        return NULL;
    }

    utils.bin.len = s.st_size;
    utils.bin.content = malloc(sizeof(char) * (utils.bin.len + 1));
    if (!utils.bin.content) {
        KILL("could not allocate memory");
        return NULL;
    }
    r_status = read(fd, utils.bin.content, utils.bin.len);
    if (r_status == -1) {
        KILL("could not read binary");
        return NULL;
    } else utils.bin.content[r_status] = 0;

    close(fd);
    srand(time(NULL) * (intptr_t)&utils);
    __log("File successfully loaded. Seed generated.");

    return &utils;
}

