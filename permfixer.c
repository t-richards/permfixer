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

// Hardcoded user/group ids
const uid_t user_owner = 1000;
const gid_t group_owner = 100;

// Hardcoded octal permissions
const mode_t dir_perm = 02775;
const mode_t file_perm = 0664;

static inline void permfixer_fix_file(const char *path)
{
    // TODO(tom): POSIX ACLs
    chown(path, user_owner, group_owner);

    // Attempt to not clobber files with higher permission levels
    struct stat statbuf;
    stat(path, &statbuf);
    chmod(path, statbuf.st_mode | file_perm);
}

static inline void permfixer_fix_dir(const char *path)
{
    // TODO(tom): POSIX ACLs
    chown(path, user_owner, group_owner);
    chmod(path, dir_perm);
}

static int permfixer_process(const char *fpath, const struct stat *sb, int tflag, struct FTW *ftwbuf)
{
    switch (tflag) {
    case FTW_F:
        printf("(file) %s\n", fpath);
        permfixer_fix_file(fpath);
        break;
    case FTW_D:
        printf("(dir)  %s\n", fpath);
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
    char *path = NULL;
    int flags = FTW_PHYS | FTW_MOUNT;

    if (argc <= 1) {
        printf("Usage: %s <directory path>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    path = realpath(argv[1], NULL);
    if (path == NULL) {
        perror("Failed to resolve path");
        exit(EXIT_FAILURE);
    }
    printf("Fixing %s\n", path);

    permfixer_fix_dir(path);
    if (nftw(path, permfixer_process, 20, flags) == -1) {
        perror("NFTW");
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}