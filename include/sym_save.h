#ifndef SYM_SAVE
#define SYM_SAVE

#include "rb_tree.h"
#include <stdlib.h>

#ifndef SYMTAB_POOL_CAPACITY
#define SYMTAB_POOL_CAPACITY 4096
#endif

#ifndef SYM_SAVE_BUMP_SIZE
#define SYM_SAVE_BUMP_SIZE 4096
#endif

#define SYM_SAVE_LITERAL(s) (sym_save_static(s, sizeof(s) - 1))

void sym_save_init (void);
const char *sym_save (rb_node **root, const char *s, size_t s_len);
const char *sym_save_static (const char *s, size_t len);

#endif
