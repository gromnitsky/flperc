#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>


void trim(char *s) {
  char *p = s;
  int l = strlen(p);
  if (!l) return;

  while (isspace(p[l - 1])) p[--l] = 0;
  while (*p && isspace(*p)) ++p, --l;
  memmove(s, p, l + 1);
}

bool each_line(char *spec, bool (*callback)(char*, char*)) {
  char *fname = NULL;
  size_t _ = 0;
  bool match = false;

  while (getline(&fname, &_, stdin) != -1) {
    trim(fname);
    match = callback(spec, fname);
  }
  free(fname);
  return match;
}

bool spec_check(char type, char *fname) {
  printf("%s: %c\n", fname, type);
  return false;
}

bool each_char(char *spec, char *fname) {
  char *type = spec;
  while (*type) {
    if (spec_check(*type++, fname)) return true;
  }
  return false;
}

int main(int argc, char **argv) {
  char *spec = "bcdpfls";
  return !each_line(spec, each_char);
}
