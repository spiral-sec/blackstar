
#include "crypter.h"
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>

static crypter_t parse_args(char **av)
{
    crypter_t args = {
        .key = strdup(av[1]),
        .target_section_name = strdup(av[2]),
        .key_section_name = strdup(av[3]),
        .target_binary = strdup(av[4]),
    };

    return args;
}

int process(crypter_t c)
{
    c.fd = open(c.target_binary, O_RDWR | O_APPEND);
    if (c.fd == -1) {
        perror("open:");
        return 1;
    }
    encrypt(c);
    return 0;
}

int main(int ac, char **av)
{
    if (ac != 5) {
        fprintf(stderr,
        "%s: Invalid number of arguments. \
        Expected %s ELF_KEY TARGET_SECTION KEY_SECTION ./binary\n",
        av[0], av[0]);
        return 0;
    } else return process(parse_args(av));
}
