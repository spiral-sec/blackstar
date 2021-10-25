
#include "blackstar.h"

#include <elf.h>
#include <string.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

blackstar_t *bl_read(char const *filepath)
{
    struct stat st = {0};
    int fd = 0;
    blackstar_t *bstar = malloc(sizeof(blackstar_t));

    if (!filepath || !bstar)
        return NULL;
    fd = open(filepath, O_RDONLY);
    if (stat(filepath, &st) < 0 || fd == -1)
        return NULL;
    bstar->path = strdup(filepath);
    bstar->content_len = st.st_size;
    bstar->content = malloc(sizeof(unsigned char *) * (st.st_size + 1));
    if (!bstar->content || read(fd, bstar->content, st.st_size) < 0)
        return NULL;
    bstar->content[st.st_size] = 0;
    close(fd);
    return bstar;
}

void bl_destroy(blackstar_t *bstar)
{
    free(bstar->content);
    free(bstar->path);
    free(bstar);
}

int bl_sync(blackstar_t *bstar)
{
    int fd = 0;

    if (!bstar || !bstar->path || unlink(bstar->path) < 0)
        return 1;
    fd = open(bstar->path, O_CREAT | O_TRUNC | O_RDWR, S_IRWXU);
    if (fd == -1 || !bstar->content || write(fd, bstar->content,
                bstar->content_len) < 0)
        return 1;
    close(fd);
    return 0;
}

Elf64_Shdr *bl_find_section(void *hdr, char const *name)
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

