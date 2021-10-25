
#include "blackstar.h"
#include <stdio.h>

int main(int ac, char **av)
{
    blackstar_t *bstar = NULL;

    if (ac != 6) {
        fprintf(stderr, "%s: Invalid number of arguments. \
        Expected %s ELF_KEY CODE_SECTION KEY_SECTION BOOL_SECTION" \
        " ./binary\n", av[0], av[0]);
        return 1;
    }
    bstar = bl_read(av[5]);
    if (!bstar)
        return 1;
    bl_encrypt_section(bstar, av[2], av[3], av[4], &xor_crypt, av[1]);
    bl_destroy(bstar);
    return 0;
}
