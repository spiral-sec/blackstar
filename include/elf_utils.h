
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

#include "setup.h"

extern char **environ;

#define ELF_KEY (".st_peter")
#define ELF_CODE (".banshee")
#define ELF_BOOL (".isalive")

/*
   To get the func section size, we can use
        size -d -A --radix=16 [binary]
    to get ELF_CODE's section size.
*/
#define ELF_FUNC_SIZE (146)

#define SECTION(x) __attribute__((section(x)))

static SECTION(ELF_BOOL) bool is_first_time = true;
static SECTION(ELF_KEY) char key[ELF_FUNC_SIZE];

typedef struct utl_s {
    int s_offset;
    int s_len;

    char *name;
    unsigned int content_len;
    unsigned char *content;
} elf_utils_t;

// elf_utils.c
void elf_save_status(elf_utils_t *);
void elf_destroy(elf_utils_t *);
elf_utils_t *elf_read(char const *);
void elf_decode(elf_utils_t *);
void generate_first_time_key(elf_utils_t *);

// payload.c
void setup_payload(settings_t *);

#endif // ELF_UTILS_H
