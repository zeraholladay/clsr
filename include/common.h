#ifndef COMMON_H
#define COMMON_H

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
debugging
*/

#ifndef DEBUG
#define DEBUG_ENABLED 0
#endif

#ifndef DEBUG
#define DEBUG(msg)                                                             \
  do {                                                                         \
    if (DEBUG_ENABLED)                                                         \
      fprintf(stderr, "[%s:%d] %s(): %s\n", __FILE__, __LINE__, __func__,      \
              msg);                                                            \
  } while (0)
#endif

/* LOCATION */

#define STRINGIFY_HELPER(x) #x
#define STRINGIFY(x) STRINGIFY_HELPER(x)
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
  safe_alloc_check((ptr) = realloc(ptr, (count) * sizeof *(ptr)))

#define FREE(ptr) ((void)(free(ptr), (ptr) = NULL))

static inline void die(const char *msg) {
  perror(msg);
  exit(1);
  abort();
}

#define container_of(ptr, type, member)                                        \
  ((type *)((char *)(ptr) - offsetof(type, member)))

/* Strings */

// size_t strnlen(const char *s, size_t maxlen) {
//   /* Do not use memchr, because on some platforms memchr has
//      undefined behavior if MAXLEN exceeds the number of bytes in S.  */
//   size_t i;
//   for (i = 0; i < maxlen && s[i]; i++)
//     continue;
//   return i;
// }

// char *strndup(char const *s, size_t n) {
//   size_t len = strnlen(s, n);
//   char *new = malloc(len + 1);

//   if (new == NULL)
//     return NULL;

//   new[len] = '\0';
//   return memcpy(new, s, len);
// }

// inline static int strncmp_minlen(const char *s1, const char *s2, size_t s1_n)
// {
//   size_t len = strlen(s2) + 1;
//   return strncmp(s1, s2, s1_n < len ? s1_n : len);
// }

#endif
