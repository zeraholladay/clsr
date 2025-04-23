#ifndef COMMON_H
#define COMMON_H

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
debugging
*/

#define INFO_ENABLED 1
#define DEBUG_ENABLED 1
#define TRACE_ENABLED 1

#ifndef INFO
#define INFO(...)                                                              \
  do {                                                                         \
    if (INFO_ENABLED)                                                          \
      fprintf(stderr, "[INFO] " __VA_ARGS__);                                  \
  } while (0)
#endif

#ifndef DEBUG
#define DEBUG(...)                                                             \
  do {                                                                         \
    if (TRACE_ENABLED)                                                         \
      fprintf(stderr, "[DEBUG] " __VA_ARGS__);                                 \
  } while (0)
#endif

#ifndef TRACE
#define TRACE(...)                                                             \
  do {                                                                         \
    if (DEBUG_ENABLED)                                                         \
      fprintf(stderr, "[TRACE] " __VA_ARGS__);                                 \
  } while (0)
#endif

/* LOCATION */

#define STRINGIFY(x) STRINGIFY2(x)
#define STRINGIFY2(x) #x
#define LOCATION __FILE__ ":" STRINGIFY(__LINE__)

/*
memory management
Gnulib - The GNU Portability Library
https://git.savannah.gnu.org/git/gnulib.git
*/

inline static int safe_alloc_check(void *ptr) {
  /* Return 0 if the allocation was successful, -1 otherwise.  */
  return -!ptr;
}

#define ALLOC(ptr) ALLOC_N(ptr, 1)

#define ALLOC_N(ptr, count)                                                    \
  safe_alloc_check((ptr) = calloc(count, sizeof *(ptr)))

#define REALLOC_N(ptr, count)                                                  \
  safe_alloc_check((ptr) = realloc(ptr, count * sizeof *(ptr)))

#define FREE(ptr) ((void)(free(ptr), (ptr) = NULL))

static inline void die(const char *msg) {
  perror(msg);
  exit(1);
  abort();
}

#define container_of(ptr, type, member)                                        \
  ((type *)((char *)(ptr) - offsetof(type, member)))

/* Strings */

inline static int strncmp_minlen(const char *s1, const char *s2, size_t s1_n) {
  size_t len = strlen(s2) + 1;
  return strncmp(s1, s2, s1_n < len ? s1_n : len);
}

const char *str_intern(const char *s, size_t s_len);

#endif
