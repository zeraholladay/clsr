#ifndef ENVIRON_H
#define ENVIRON_H

#include <string.h>

#ifndef MAX_ENTRIES
#define MAX_ENTRIES 16
#endif

#include "object.h"

struct _env_entry {
  const char *symbol; // symbol name
  int addr;           // heap address
};

typedef struct _env_t {
  struct _env_entry entries[MAX_ENTRIES];
  int count;
  struct _env_t *parent;
} env_t;

int lookup_env(env_t *env, const char *s, size_t s_len);

#endif
