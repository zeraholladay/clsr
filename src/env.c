#include <stdlib.h>

#include "env.h"
#include "oom_handlers.h"
#include "safe_str.h"

#ifndef ENV_STR_MAX
#define ENV_STR_MAX 256
#endif

extern oom_handler_t env_oom_handler;

Env *env_new(Env *parent) {
  struct Env *env = calloc(1, sizeof *(env));

  if (!env) {
    env_oom_handler(NULL, OOM_LOCATION);
    return NULL;
  }

  env->parent = parent;
  return env;
}

int env_lookup(Env *env, const char *sym, void **addr) {
  size_t len = safe_strnlen(sym, ENV_STR_MAX);

  for (Env *cur_env = env; cur_env; cur_env = cur_env->parent) {
    rb_node *node = rb_lookup(cur_env->root, sym, len);

    if (node) {
      *addr = RB_VAL(node);
      return 0;
    }
  }

  return -1; // fail
}

int env_set(Env *env, const char *sym, void *addr) {
  size_t len = safe_strnlen(sym, ENV_STR_MAX);

  for (Env *cur_env = env; cur_env; cur_env = cur_env->parent) {
    rb_node *node = rb_lookup(cur_env->root, sym, len);

    if (node) {
      RB_VAL(node) = addr;
      return 0;
    }
  }

  rb_node *node = rb_alloc(); // FIXME

  if (!node) {
    env_oom_handler(NULL, OOM_LOCATION);
    return -1;
  }

  RB_KEY(node) = sym;
  RB_KEY_LEN(node) = len;
  RB_VAL(node) = addr;

  rb_insert(&env->root, node);

  return 0;
}
