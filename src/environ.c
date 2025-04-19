#include <stdlib.h>
#include <strings.h>

#include "common.h"
#include "environ.h"

EnvironEntry *_env_lookup(Environ *env, const char *sym, size_t sym_len);

Environ *env_new(Environ *parent) {
  struct Environ *env = NULL;
  if (ALLOC(env))
    die(LOCATION);
  memset(env, 0, sizeof(Environ));
  env->parent = parent;
  return env;
}

/* perf really matters here. */
EnvironEntry *_env_lookup(Environ *env, const char *sym, size_t sym_len) {
  for (Environ *frame = env; frame != NULL; frame = frame->parent)
    for (int i = 0; i < frame->count; i++)
      if (0 == strncmp_minlen(sym, frame->entries[i].symbol, sym_len))
        return &(frame->entries[i]);
  return NULL;
}

int env_set(Environ *env, const char *sym, int addr) {
  if (env->count >= MAX_ENTRIES) {
    die(LOCATION);
  }
  EnvironEntry *entity = _env_lookup(env, sym, addr);
  if (entity)
    entity->addr = addr;
  else {
    env->entries[env->count].symbol = sym;
    env->entries[env->count].addr = addr;
    env->count++;
  }
  return 1;
}

int env_lookup(Environ *env, const char *sym, size_t sym_len) {
  EnvironEntry *entity = _env_lookup(env, sym, sym_len);
  if (entity)
    return entity->addr;
  return -1;
}
