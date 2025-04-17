#ifndef ENVIRON_H
#define ENVIRON_H

#include <stddef.h>

#ifndef MAX_ENTRIES
#define MAX_ENTRIES 16
#endif

#include "object.h"

typedef struct env_entry_t {
  const char *symbol; // symbol name
  int addr;           // heap address
} env_entry_t;

typedef struct env_t {
  env_entry_t entries[MAX_ENTRIES];
  int count;
  struct env_t *parent;
} env_t;

env_t *env_new(env_t *parent);
int env_set(env_t *env, const char *sym, size_t sym_len, int addr);
int lookup_env(env_t *env, const char *s, size_t s_len);

#endif
