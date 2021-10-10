
#ifndef PAYLOAD_H
#define PAYLOAD_H

#include "elf_utils.h"

#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <errno.h>
#include <strings.h>
#include <sys/select.h>
#include <unistd.h>
#include <linux/input.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdbool.h>
#include <stdbool.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <linux/input.h>
#include <errno.h>
#include <unistd.h>
#include <limits.h>

#define BUFFER_SIZE 32
#define NB_KEYCODES 84
#define NB_EVENTS   512

#define EV_KEY_PRESSED 1
#define EV_KEY_RELEASE 0

#define DIR_PATH ("/dev/input/by-path/")

// keyboard.c
int *find_keyboards(void);

#endif // PAYLOAD_H
