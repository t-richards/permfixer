#define _XOPEN_SOURCE 500

#include <errno.h>
#include <ftw.h>
#include <getopt.h>
#include <grp.h>
#include <limits.h>
#include <pwd.h>
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

// Other globals
char *myname = NULL;

static struct option longopts[] = {{"dperm", required_argument, NULL, 'd'},
                                   {"fperm", required_argument, NULL, 'f'},
                                   {"group", required_argument, NULL, 'g'},
                                   {"help", no_argument, NULL, 'h'},
                                   {"user", required_argument, NULL, 'u'},
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

static mode_t permfixer_parse_perms(const char *perm, const char *type) {
  mode_t val;
  char *ep;

  errno = 0;
  val = strtoul(perm, &ep, 8);
  if (errno || *ep != '\0') {
    fprintf(stderr, "%s: illegal %s permissions\n", perm, type);
    exit(EXIT_FAILURE);
  }

  return val;
}

static uid_t id(const char *name, const char *type) {
  uid_t val;
  char *ep;

  /*
   * XXX
   * We know that uid_t's and gid_t's are unsigned longs.
   */
  errno = 0;
  val = strtoul(name, &ep, 10);
  if (errno || *ep != '\0') {
    fprintf(stderr, "%s: illegal %s name\n", name, type);
    exit(EXIT_FAILURE);
  }

  return val;
}

static uid_t permfixer_parse_uid(const char *s) {
  struct passwd *pw;

  if (s == NULL || *s == '\0') {
    fprintf(stderr, "Empty user provided\n.");
    exit(EXIT_FAILURE);
  }

  pw = getpwnam(s);
  if (pw != NULL) {
    return pw->pw_uid;
  }

  return id(s, "user");
}

static gid_t permfixer_parse_gid(const char *s) {
  struct group *gr;

  if (s == NULL || *s == '\0') {
    fprintf(stderr, "Empty group provided\n.");
    exit(EXIT_FAILURE);
  }

  gr = getgrnam(s);
  if (gr != NULL) {
    return gr->gr_gid;
  }

  return id(s, "group");
}

static void usage(void) {
  printf("usage: %s [opts] <directory path>\n", myname);
  printf("options:\n");
  printf("  -h, --help           Show this help message\n");
  printf("  -d, --dperm <perm>   Specify directory permissions\n");
  printf("                       Current value: %04o\n", dir_perm);
  printf("  -f, --fperm <perm>   Specify file permissions\n");
  printf("                       Current value: %04o\n", file_perm);
  printf("  -g, --group <group>  Specify an alternate group owner\n");
  printf("                       Current value: %d\n", group_owner);
  printf("  -u, --user <user>    Specify an alternate user owner\n");
  printf("                       Current value: %d\n", user_owner);
}

int main(int argc, char *argv[]) {
  // init
  myname = argv[0];

  // Application data
  char *path = NULL;
  int flags = FTW_PHYS | FTW_MOUNT;
  user_owner = getuid();
  group_owner = getgid();

  // getopt junk
  int ch;
  char *p;

  while ((ch = getopt_long(argc, argv, "dfghu:", longopts, NULL)) != -1) {
    switch (ch) {
    case 'd':
      dir_perm = permfixer_parse_perms(optarg, "directory");
      break;
    case 'f':
      file_perm = permfixer_parse_perms(optarg, "file");
      break;
    case 'g':
      group_owner = permfixer_parse_gid(optarg);
      break;
    case 'h':
      usage();
      exit(EXIT_SUCCESS);
    case 'u':
      user_owner = permfixer_parse_uid(optarg);
      break;
    case '?':
    default:
      usage();
      exit(EXIT_FAILURE);
    }
  }

  if (optind >= argc) {
    fprintf(stderr, "error: no directory path specified (use -h for help)\n");
    exit(EXIT_FAILURE);
  }

  path = realpath(argv[optind], NULL);
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
