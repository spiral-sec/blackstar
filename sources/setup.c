
#include "setup.h"
#include <asm-generic/socket.h>
#include <bits/getopt_core.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <getopt.h>
#include <unistd.h>

static bool debugger_is_attached(void)
{
    char buf[4096] = {0};

    __log("Seeking debugger");
    const int status_fd = open("/proc/self/status", O_RDONLY);
    if (status_fd == -1) {
        KILL("could not open /proc/self/status");
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
            __log("found debugger, exiting");
            return isdigit(*char_ptr) != 0 && *char_ptr != '0';
        }
    }
    __log("No debugger found");
    return false;
}

static void setup_log(void)
{
    g_settings.logfd = open(LOG_PATH, O_WRONLY | O_TRUNC | O_CREAT, S_IRUSR);
    __log("Log file setup");
}

static void setup_socket(void)
{
    int flag = 1;
    struct sockaddr_in addr = {
        .sin_family = AF_INET,
        .sin_port = htons(atoi(g_settings.target_ip)),
        .sin_addr.s_addr = inet_addr(g_settings.target_port)
    };

    g_settings.sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (!g_settings.sockfd)
        return;
    else if (setsockopt(g_settings.sockfd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
        &flag, sizeof(int)) < 0)
        return;
    else if (connect(g_settings.sockfd, (struct sockaddr *)&addr, sizeof(addr)) < 0)
        return;
    __log("Successfully connected to server");
}

static void daemonize(void)
{
    register int file_desc = sysconf(_SC_OPEN_MAX);

    if (setsid() < 0 || fork() < 0)
        exit(1);
    umask(0);
    while (file_desc-- >= 0)
        close(file_desc);
}

void __log(char const *s)
{
    if (g_settings.sockfd != -1)
        dprintf(g_settings.sockfd, "%s\n", s);
    if (g_settings.logfd != -1)
        dprintf(g_settings.sockfd, "%s\n", s);
}

void flush(void)
{
    __log("good bye");
    close(g_settings.sockfd);
    close(g_settings.logfd);
    _exit(1);
}

void init(void)
{
    signal(SIGCHLD, SIG_IGN);
    signal(SIGHUP, SIG_IGN);
    signal(SIGTRAP, SIG_IGN);

    daemonize();
    setup_log();
    setup_socket();

    if (debugger_is_attached())
        exit(1);
}

void parse_settings(int ac, char *const *av)
{
    register int c = '?';

    while ((c = getopt(ac, av, "+:i:p")) != -1) {
        switch (c) {
            case 'i':
                strncpy(g_settings.target_ip, optarg, 128);
                break;
            case 'p':
                strncpy(g_settings.target_port, optarg, 128);
                break;
            default:
                KILL("unexpected argument");
        }
    }
    if (*g_settings.target_ip == 0 || *g_settings.target_port == 0)
        KILL("a value was not set");
}
