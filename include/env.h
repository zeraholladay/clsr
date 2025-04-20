#ifndef ENVIRON_H
#define ENVIRON_H

#include <stddef.h>

#ifndef MAX_ENTRIES
#define MAX_ENTRIES 16
#endif

#include "obj.h"

typedef struct EnvEntry {
  const char *symbol; // symbol name
  int addr;           // heap address
} EnvEntry;

typedef struct Env {
  EnvEntry entries[MAX_ENTRIES];
  int count;
  struct Env *parent;
} Env;

Env *env_new(Env *parent);
int env_set(Env *env, const char *sym, int addr);
int env_lookup(Env *env, const char *s, size_t s_len);

#endif
