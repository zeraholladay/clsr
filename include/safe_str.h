#ifndef SAFE_STR
#define SAFE_STR

#include <stdlib.h>
#include <string.h>

inline static size_t safe_strnlen(const char *s, size_t maxlen) {
  size_t i;
  for (i = 0; i < maxlen && s[i]; i++)
    continue;
  return i;
}

inline static char *safe_strndup(char const *s, size_t n) {
  size_t len = safe_strnlen(s, n);
  char *new = (char *)malloc(len + 1);

  if (new == NULL)
    return NULL;

  new[len] = '\0';
  return memcpy(new, s, len);
}

inline static int safe_strncmp_minlen(const char *s1, const char *s2,
                                      size_t n) {
  size_t len1 = safe_strnlen(s1, n);
  size_t len2 = safe_strnlen(s2, n);

  int cmp = memcmp(s1, s2, len1 < len2 ? len1 : len2);
  if (cmp != 0 || len1 == len2)
    return cmp;
  return (len1 < len2) ? -1 : 1;
}

#endif
