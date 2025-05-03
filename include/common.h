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

static inline void die(const char *msg) {
  perror(msg);
  exit(1);
  abort();
}

#endif
