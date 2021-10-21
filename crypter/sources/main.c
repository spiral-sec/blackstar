
#include "crypter.h"
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

static crypter_t parse_args(char **av)
{
    crypter_t args = {
        .key = strdup(av[1]),
        .target_section_name = strdup(av[2]),
        .bool_section_name = strdup(av[3]),
        .target_binary = strdup(av[4]),
    };

    return args;
}

int main(int ac, char **av)
{
    crypter_t args = {0};

    if (ac != 5) {
        fprintf(stderr,
        "%s: Invalid number of arguments. \
        Expected %s ELF_KEY TARGET_SECTION BOOLEAN_SECTION ./binary\n",
        av[0], av[0]);
        return 0;
    } else args = parse_args(av);

    return 0;
}
