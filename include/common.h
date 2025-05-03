#ifndef COMMON_H
#define COMMON_H

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef CLSR_DEBUG
#define CLSR_DEBUG_ENABLED 1
#else
#define CLSR_DEBUG_ENABLED 0
#endif

#ifndef debug
#define debug(msg)                                                             \
  do {                                                                         \
    if (CLSR_DEBUG_ENABLED)                                                    \
      fprintf(stderr, "[%s:%d] %s(): %s\n", __FILE__, __LINE__, __func__,      \
              msg);                                                            \
  } while (0)
#endif

#define STRINGIFY_HELPER(x) #x
#define STRINGIFY(x) STRINGIFY_HELPER(x)
#define LOCATION "[" __FILE__ ":" STRINGIFY(__LINE__) "] "

/*
memory and string management
Gnulib - The GNU Portability Library
https://git.savannah.gnu.org/git/gnulib.git
*/

#define container_of(ptr, type, member)                                        \
  ((type *)((char *)(ptr) - offsetof(type, member)))

inline static int safe_alloc_check(void *ptr) {
  /* Return 0 if the allocation was successful, -1 otherwise.  */
  return -!ptr;
}

#define ALLOC_N(ptr, count)                                                    \
  safe_alloc_check((ptr) = calloc(count, sizeof *(ptr)))

#define ALLOC(ptr) ALLOC_N(ptr, 1)

#define REALLOC_N(ptr, count)                                                  \
  safe_alloc_check((ptr) = realloc(ptr, (count) * sizeof *(ptr)))

#define FREE(ptr) ((void)(free(ptr), (ptr) = NULL))

static inline void die(const char *msg) {
  perror(msg);
  exit(1);
  abort();
}

inline static size_t safe_strnlen(const char *s, size_t maxlen) {
  /* Do not use memchr, because on some platforms memchr has
     undefined behavior if MAXLEN exceeds the number of bytes in S.  */
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
