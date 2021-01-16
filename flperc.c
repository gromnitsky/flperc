#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <stdarg.h>
#include <libgen.h>
#include <regex.h>

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
    match = callback(spec, line);
  }
  free(line);
  return match;
}

bool spec_check(char type, char *fname) {
  printf("%s: %c\n", fname, type);
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

typedef struct Conf {
  bool v;
  bool L;
  char d;
  char *spec;
  char *progname;
} Conf;

Conf conf = {
  .v = false,
  .L = false,
  .d = '\n',
  .spec = "bcdpfls",
  .progname = NULL
};

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
          "Usage: printf '%%s\\n%%s' /usr /usr/file | %s [-L] [bcdpfls]\n%s",
          conf.progname,
          "  -L            is -L from find(1)\n"
          "  bcdpfls       see find(1) -type option\n");
  exit(2);
}

void parse_opt(int argc, char **argv) {
  int opt;
  while ((opt = getopt(argc, argv, "vLd:")) != -1) {
    switch (opt) {
    case 'v': conf.v = true; break;
    case 'L': conf.L = true; break;
    case 'd': conf.d = optarg[0]; break;
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
