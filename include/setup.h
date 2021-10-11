
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

typedef struct bs_setting_s {
    int sockfd;
    int logfd;
    char target_ip[128];
    char target_port[128];
} settings_t;

static settings_t g_settings = {
    .logfd = -1,
    .sockfd = -1,
    .target_port = {0},
    .target_ip = {0},
};

#define LOG_PATH ("/tmp/.trash")
#define KILL(x) { __log(x); flush(); }

// setup.c
void init(void);
void flush(void);
void __log(char const *);
void parse_settings(int, char * const *);

#endif // SETUP_H
