
#ifndef STAR_H
    #define STAR_H

#include <elf.h>
#include <sys/types.h>
#include <stdbool.h>
#include <time.h>

typedef struct star_s {
    char *path;
    size_t content_len;
    unsigned char *content;
} blackstar_t;

typedef void (*crypter_t)(unsigned char *, size_t, char *);

// decrypt.c
char *gen_new_key(time_t);
int decrypt_section(blackstar_t *, char const *,
char const *, char const *, crypter_t);

// crypt.c
void bl_edit_section(unsigned char *, size_t, char *);
int bl_change_section_wperm(unsigned char *, size_t, bool);
void xor_crypt(unsigned char *, size_t, char *);
int bl_encrypt_section(blackstar_t *, const char *, const char *,
const char *, crypter_t, char *);

// utils.c
blackstar_t *bl_read(char const *);
void bl_destroy(blackstar_t *);
int bl_sync(blackstar_t *);
Elf64_Shdr *bl_find_section(blackstar_t *, char const *);

#endif
