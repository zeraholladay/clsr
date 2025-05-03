#include <stdlib.h>
#include <string.h>

#include "oom_handlers.h"
#include "palloc.h"
#include "rb_tree.h"
#include "sym_save.h"

extern sym_save_oom_handler_t sym_save_oom_handler;

typedef struct BumpPool {
  size_t offset;
  struct BumpPool *next;
  char buffer[SYM_SAVE_BUMP_SIZE];
} BumpPool;

static Pool *pool = NULL;
static BumpPool *bump_pool = NULL;

static BumpPool *bump_pool_new(void) {
  BumpPool *new = calloc(1, sizeof *(bump_pool));

  if (!new) {
    sym_save_oom_handler(NULL, OOM_LOCATION);
    return NULL;
  }
  new->offset = 0;
  new->next = NULL;
  return new;
}

static char *bump_pool_alloc(size_t n) {
  if (bump_pool->offset + n > SYM_SAVE_BUMP_SIZE) {
    BumpPool *new = bump_pool_new();
    new->next = bump_pool;
    bump_pool = new;
  }
  char *ptr = &bump_pool->buffer[bump_pool->offset];
  bump_pool->offset += n;
  return ptr;
}

static char *bump_pool_strndup(const char *s, size_t n) {
  size_t len = n + 1;
  char *new = bump_pool_alloc(len);
  new[len] = '\0';
  return memcpy(new, s, len);
}

void sym_save_init(void) {
  bump_pool = bump_pool_new();
  pool = pool_init(SYMTAB_POOL_COUNT, sizeof(rb_node));
}

const char *sym_save(rb_node **root, const char *s, size_t n) {
  rb_node *node = rb_lookup(*root, s, n);

  if (node)
    return RB_KEY(node);

  node = pool_alloc(pool);

  if (!node)
    return NULL;

  RB_KEY(node) = bump_pool_strndup(s, n);
  RB_KEY_LEN(node) = n;
  // note: no RB_VAL here. ie symbols don't have values.
  RB_VAL(node) = NULL;

  if (!RB_KEY(node))
    return NULL;

  rb_insert(root, node);

  return RB_KEY(node);
}
