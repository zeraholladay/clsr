#ifndef STRUTIL_H
#define STRUTIL_H

#include <string.h>

inline static int strncmp_minlen(const char *s1, const char *s2, size_t s1_n) {
  size_t len = strlen(s2) + 1;
  return strncmp(s1, s2, s1_n < len ? s1_n : len);
}

const char *sym_intern(const char *s, size_t s_len);

#endif