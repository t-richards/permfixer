#define _XOPEN_SOURCE 500

#include <errno.h>
#include <ftw.h>
#include <getopt.h>
#include <limits.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/acl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

// User / group ids
uid_t user_owner;
gid_t group_owner;

// Directory / file permissions
mode_t dir_perm = 0755;
mode_t file_perm = 0644;

static struct option longopts[] = {{"dperm", required_argument, NULL, 'd'},
                                   {"fperm", required_argument, NULL, 'f'},
                                   {"gid", required_argument, NULL, 'g'},
                                   {"help", no_argument, NULL, 'h'},
                                   {"uid", required_argument, NULL, 'u'},
                                   {NULL, 0, NULL, 0}};

// Fixes permissions on a single file
static inline void permfixer_fix_file(const char *path) {
  // Change ownership of file
  if (chown(path, user_owner, group_owner) == -1) {
    fprintf(stderr, "Error changing ownership of %s: %s\n", path,
            strerror(errno));
    exit(EXIT_FAILURE);
  }

  // Upgrade permissions of file
  // Attempt to not clobber files with higher permission levels
  struct stat statbuf;
  stat(path, &statbuf);
  if (chmod(path, statbuf.st_mode | file_perm) == -1) {
    fprintf(stderr, "Error changing permissions of %s: %s\n", path,
            strerror(errno));
    exit(EXIT_FAILURE);
  }
}

// Fixes permissions on a single directory
static inline void permfixer_fix_dir(const char *path) {
  // Change owner of directory
  if (chown(path, user_owner, group_owner) == -1) {
    fprintf(stderr, "Error changing ownership of %s: %s\n", path,
            strerror(errno));
    exit(EXIT_FAILURE);
  }

  // Change permissions of directory
  if (chmod(path, dir_perm) == -1) {
    fprintf(stderr, "Error changing permissions of %s: %s\n", path,
            strerror(errno));
    exit(EXIT_FAILURE);
  }
}

// Callback function for nftw()
static int permfixer_process(const char *fpath, const struct stat *sb,
                             int tflag, struct FTW *ftwbuf) {
  switch (tflag) {
  case FTW_F:
    permfixer_fix_file(fpath);
    break;
  case FTW_D:
    permfixer_fix_dir(fpath);
    break;
  }

  return 0;
}

void usage(const char *const myname) {
  printf("usage: %s [opts] <directory path>\n", myname);
  printf("options:\n");
  printf("  -h, --help           Show this help message\n");
  printf("  -d, --dperm <perm>   Specify directory permissions\n");
  printf("                       Default: %04o\n", dir_perm);
  printf("  -f, --fperm <perm>   Specify file permissions\n");
  printf("                       Default: %04o\n", file_perm);
  printf("  -g, --group <group>  Specify an alternate group\n");
  printf("                       Default: %d\n", group_owner);
  printf("  -o, --owner <owner>  Specify an alternate owner\n");
  printf("                       Default: %d\n", user_owner);
}

int main(int argc, char *argv[]) {
  // Application data
  char *path = NULL;
  int flags = FTW_PHYS | FTW_MOUNT;
  user_owner = geteuid();
  group_owner = getegid();

  // getopt junk
  int ch;
  char *p;

  while ((ch = getopt_long(argc, argv, "dfghu:", longopts, NULL)) != -1) {
    switch (ch) {
    case 'd':
      printf("dperm: %s\n", optarg);
      break;
    case 'f':
      printf("fperm: %s\n", optarg);
      break;
    case 'g':
      printf("gid: %s\n", optarg);
      break;
    case 'h':
      usage(argv[0]);
      exit(EXIT_SUCCESS);
    case 'u':
      printf("uid: %s\n", optarg);
      break;
    }
  }

  exit(EXIT_SUCCESS);

  path = realpath(argv[1], NULL);
  if (path == NULL) {
    fprintf(stderr, "Failed to resolve path %s: %s\n", argv[1],
            strerror(errno));
    exit(EXIT_FAILURE);
  }

  if (strcmp(path, "/") == 0) {
    fprintf(stderr, "Refusing to run on /");
    exit(EXIT_FAILURE);
  }

  printf("Fixing %s... ", path);
  fflush(stdout);

  if (nftw(path, permfixer_process, 20, flags) == -1) {
    fprintf(stderr, "Failed to walk file tree.\n");
    exit(EXIT_FAILURE);
  }

  puts("done.");

  exit(EXIT_SUCCESS);
}
