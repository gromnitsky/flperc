#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdarg.h>
#include <libgen.h>
#include <regex.h>

typedef struct Conf {
  char *version;
  bool v;
  bool L;
  char d;
  char *spec;
  char *progname;
} Conf;

Conf conf = {
  .version = "0.0.1",
  .v = false,
  .L = false,
  .d = '\n',
  .spec = "bcdpfls",
  .progname = NULL
};

void trim_right(char delim, char *s) {
  char *pos;
  if ( (pos=strchr(s, delim)) != NULL) *pos = '\0';
}

bool each_delim(char delim, char *spec, bool (*callback)(char*, char*)) {
  char *line = NULL;
  size_t _ = 0;
  bool match = false;

  while (getdelim(&line, &_, delim, stdin) != -1) {
    trim_right(delim, line);
    if (callback(spec, line)) {
      printf("%s\n", line);
      match = true;
    }
  }
  free(line);
  return match;
}

bool spec_check(char type, char *fname) {
  struct stat sb;
  int (*relevant_stat)(const char*, struct stat *) = lstat;

  if (conf.v) fprintf(stderr, "* %s  %c\n", fname, type);
  if (conf.L) relevant_stat = stat;
  if (relevant_stat(fname, &sb) != 0) return false;

  switch (type) {
  case 'b': return S_ISBLK(sb.st_mode);  // block special
  case 'c': return S_ISCHR(sb.st_mode);  // character special
  case 'd': return S_ISDIR(sb.st_mode);  // directory
  case 'p': return S_ISFIFO(sb.st_mode); // named pipe
  case 'f': return S_ISREG(sb.st_mode);  // file
  case 'l': return S_ISLNK(sb.st_mode);  // symlink
  case 's': return S_ISSOCK(sb.st_mode); // socket
  }
  return false;
}

bool each_char(char *spec, char *fname) {
  if (!strlen(fname)) return false;
  char *type = spec;
  while (*type) {
    if (spec_check(*type++, fname)) return true;
  }
  return false;
}

void err(const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);
  fprintf(stderr, "%s error: ", conf.progname);
  vfprintf(stderr, fmt, args);
  va_end(args);
  exit(2);
}

void usage() {
  fprintf(stderr,
          "Usage: printf '%%s\\n%%s' /usr /usr/file | %s [-0L] [-d char] [bcdpfls]\n%s",
          conf.progname,
          "-L         is -L from find(1)\n"
          "-d CHAR    a custom delimiter instead of a newline\n"
          "-0         a shortcut for -d $'\\0'\n"
          "-V         print the program version\n"
          "\n"
          "bcdpfls    see find(1) -type option\n");
  exit(2);
}

void parse_opt(int argc, char **argv) {
  int opt;
  while ((opt = getopt(argc, argv, "vLd:0V")) != -1) {
    switch (opt) {
    case 'V': printf("%s\n", conf.version); exit(0);
    case 'v': conf.v = true; break;
    case 'L': conf.L = true; break;
    case 'd': conf.d = optarg[0]; break;
    case '0': conf.d = '\0'; break;
    default: usage();
    }
  }
  if (argc > optind) conf.spec = argv[optind];
}

bool re_match(char *pattern, const char *s) {
  int status;
  regex_t re;
  if (regcomp(&re, pattern, REG_EXTENDED|REG_NOSUB) != 0) return false;
  status = regexec(&re, s, 0, NULL, 0);
  regfree(&re);
  return !status;
}

int main(int argc, char **argv) {
  conf.progname = basename(argv[0]);
  parse_opt(argc, argv);
  if (!re_match("^[bcdpfls]+$", conf.spec))
    err("invalid spec: %s\n", conf.spec);

  return !each_delim(conf.d, conf.spec, each_char);
}
