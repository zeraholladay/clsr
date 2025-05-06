#ifndef DEBUG_H
#define DEBUG_H

#include <stdio.h>

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

#endif
