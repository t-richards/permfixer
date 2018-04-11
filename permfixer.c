#define _XOPEN_SOURCE 500

#include <errno.h>
#include <ftw.h>
#include <limits.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int process(const char *fpath, const struct stat *sb, int tflag, struct FTW *ftwbuf)
{
    const char *name = fpath + ftwbuf->base;

    switch (tflag) {
    case FTW_F:
        printf("(file)      %s\n", name);
        break;
    case FTW_D:
        printf("(directory) %s\n", name);
        break;
    }

    return 0;
}

int main(int argc, char *argv[])
{
    int flags = FTW_DEPTH | FTW_PHYS;

    if (nftw("/home/tom/Projects/permfixer", process, 20, flags) == -1) {
        perror("NFTW");
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}