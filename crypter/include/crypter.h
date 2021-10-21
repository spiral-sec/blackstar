
#ifndef CRYPTER_H
#define CRYPTER_H

#include <stdio.h>

typedef struct crypter_args {
    char *target_binary;
    int fd;

    char *key;
    char *target_section_name;
    char *bool_section_name;
} crypter_t;


// crypt.c
void *get_memory_map(crypter_t);
void __xor(unsigned char *, char *);

#endif /* CRYPTER_H  */
