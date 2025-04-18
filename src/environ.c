#include "environ.h"
#include "log.h"
#include "strutil.h"

#include <stdlib.h>
#include <strings.h>

env_entry_t *_lookup_env(env_t *env, const char *sym, size_t sym_len);

env_t *env_new(env_t *parent) {
  struct env_t *env = malloc(sizeof(env_t));
  if (!env)
    return NULL;
  memset(env, 0, sizeof(env_t));
  env->parent = parent;
  return env;
}

/* perf really matters here. */
env_entry_t *_lookup_env(env_t *env, const char *sym, size_t sym_len) {
  for (env_t *frame = env; frame != NULL; frame = frame->parent)
    for (int i = 0; i < frame->count; i++)
      if (0 == strncmp_minlen(sym, frame->entries[i].symbol, sym_len))
        return &(frame->entries[i]);
  return NULL;
}

int env_set(env_t *env, const char *sym, size_t sym_len, int addr) {
  if (env->count >= MAX_ENTRIES) {
    ERRMSG("Environment full\n");
    return -1;
  }
  env_entry_t *entity = _lookup_env(env, sym, addr);
  if (entity)
    entity->addr = addr;
  else {
    env->entries[env->count].symbol = sym;
    env->entries[env->count].addr = addr;
    env->count++;
  }
  return 1;
}

int lookup_env(env_t *env, const char *sym, size_t sym_len) {
  env_entry_t *entity = _lookup_env(env, sym, sym_len);
  if (entity)
    return entity->addr;
  return -1;
}
