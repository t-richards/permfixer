#define _XOPEN_SOURCE 500

#include <errno.h>
#include <ftw.h>
#include <limits.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

const uid_t user_owner = 1000;
const gid_t group_owner = 100;

const mode_t file_perm = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH;
const mode_t dir_perm = S_IRWXU | S_IRWXG | S_ISGID | S_IROTH | S_IXOTH;

static inline void permfixer_fix_file(const char *path)
{
    chown(path, user_owner, group_owner);
    chmod(path, file_perm);
}

static inline void permfixer_fix_dir(const char *path)
{
    chown(path, user_owner, group_owner);
    chmod(path, dir_perm);
}

static int permfixer_process(const char *fpath, const struct stat *sb, int tflag, struct FTW *ftwbuf)
{
    switch (tflag) {
    case FTW_F:
        printf("(file)      %s\n", fpath);
        permfixer_fix_file(fpath);
        break;
    case FTW_DP:
    case FTW_D:
        printf("(directory) %s\n", fpath);
        permfixer_fix_dir(fpath);
        break;
    default:
        printf("(unknown) %d %s\n", tflag, fpath);
        break;
    }

    return 0;
}

int main(int argc, char *argv[])
{
    const char *path = "/home/tom/TMP/testdir";
    int flags = FTW_PHYS | FTW_MOUNT;

    chmod(path, dir_perm);
    if (nftw(path, permfixer_process, 20, flags) == -1) {
        perror("NFTW");
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}