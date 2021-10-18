
#ifndef SETUP_H
#define SETUP_H

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <stdbool.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define DEFAULT_PORT ("42069")
#define DEFAULT_IP ("0.0.0.0")

typedef struct bs_setting_s {
    int sockfd;
    int logfd;
    char *target_ip;
    char *target_port;
} settings_t;

#define LOG_PATH ("/tmp/blackstar.log")
#define KILL(x) { fprintf(stderr, "%s\n\n", x); exit(1); }

// setup.c
void daemonize(void);
void init(settings_t *);
void flush(settings_t);
void __log(char const *, settings_t);
void parse_settings(int, char * const *, settings_t *);

#endif // SETUP_H
