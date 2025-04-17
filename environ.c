#include "environ.h"
#include "log.h"

int lookup_env(env_t *env, const char *s, size_t s_len) {
  for (env_t *frame = env; frame != NULL; frame = frame->parent) {
    for (int i = 0; i < frame->count; i++) {
      int len = strlen(frame->entries[i].symbol) + 1;
      int min_leng = s_len < len ? s_len : len;

      if (0 == strncmp(s, frame->entries[i].symbol, min_leng)) {
        return frame->entries[i].addr;
      }
    }
  }
  ERRMSG("Unbound variable: %s\n", s);
  return -1;
}
