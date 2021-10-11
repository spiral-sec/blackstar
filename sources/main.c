
#include "elf_utils.h"
#include "setup.h"

int main(int ac, char * const *av)
{
    elf_utils_t *utils = NULL;
    pid_t pid = 0;

    parse_settings(ac, av);
    init();

    utils = elf_read(av[0]);
    if (!utils)
        exit(1);

    elf_decode(utils);
    setup_payload();
    return 0;
}
