
#ifndef CRYPTER_H
#define CRYPTER_H

#include <stdio.h>

typedef struct crypter_args {
    char *target_binary;
    int fd;

    char *key;
    char *target_section_name;
} crypter_t;


// crypt.c
void *get_memory_map(crypter_t);
void __xor(unsigned char *, size_t, char *);
void decrypt(unsigned char *, unsigned char *, char *);
void encrypt(crypter_t);

#endif /* CRYPTER_H  */
