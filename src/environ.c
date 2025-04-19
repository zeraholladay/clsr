#include <stdlib.h>
#include <strings.h>

#include "environ.h"
#include "common.h"
#include "strutil.h"

env_entry_t *_env_lookup(env_t *env, const char *sym, size_t sym_len);

env_t *env_new(env_t *parent) {
  struct env_t *env = NULL;
  if (ALLOC(env))
    die(LOCATION);
  memset(env, 0, sizeof(env_t));
  env->parent = parent;
  return env;
}

/* perf really matters here. */
env_entry_t *_env_lookup(env_t *env, const char *sym, size_t sym_len) {
  for (env_t *frame = env; frame != NULL; frame = frame->parent)
    for (int i = 0; i < frame->count; i++)
      if (0 == strncmp_minlen(sym, frame->entries[i].symbol, sym_len))
        return &(frame->entries[i]);
  return NULL;
}

int env_set(env_t *env, const char *sym, int addr) {
  if (env->count >= MAX_ENTRIES) {
    die(LOCATION);
  }
  env_entry_t *entity = _env_lookup(env, sym, addr);
  if (entity)
    entity->addr = addr;
  else {
    env->entries[env->count].symbol = sym;
    env->entries[env->count].addr = addr;
    env->count++;
  }
  return 1;
}

int env_lookup(env_t *env, const char *sym, size_t sym_len) {
  env_entry_t *entity = _env_lookup(env, sym, sym_len);
  if (entity)
    return entity->addr;
  return -1;
}
