#include "sym_save.h"
#include "palloc.h"
#include "rb_tree.h"

#include "common.h"

static Pool *pool = NULL;

void sym_save_init(void) { pool = pool_init(SYMTAB_COUNT, sizeof(rb_node)); }

const char *sym_save(rb_node **root, const char *s, size_t n) {
  rb_node *node = rb_lookup(*root, s, n);

  if (node)
    return RB_KEY(node);

  node = pool_alloc(pool);

  if (!node)
    return NULL;

  RB_KEY(node) = safe_strndup(s, n);
  RB_KEY_LEN(node) = n;
  // note: no RB_VAL here. ie symbols don't have values.
  RB_VAL(node) = NULL;

  if (!RB_KEY(node))
    return NULL;

  rb_insert(root, node);

  return RB_KEY(node);
}
