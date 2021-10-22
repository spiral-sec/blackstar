
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
        .target_binary = strdup(av[3]),
    };

    return args;
}

int process(crypter_t c)
{
    int fd = open(c.target_binary, O_RDWR | O_APPEND);

    if (fd == -1)
        return 1;
    encrypt(c);
    close(c.fd);
    return 0;
}

int main(int ac, char **av)
{
    crypter_t args = {0};

    if (ac != 4) {
        fprintf(stderr,
        "%s: Invalid number of arguments. \
        Expected %s ELF_KEY TARGET_SECTION ./binary\n",
        av[0], av[0]);
        return 0;
    } else return process(parse_args(av));
}
