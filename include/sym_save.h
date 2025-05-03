#ifndef SYM_SAVE
#define SYM_SAVE

#include "rb_tree.h"
#include <stdlib.h>

#ifndef SYMTAB_POOL_COUNT
#define SYMTAB_POOL_COUNT 4096
#endif

#ifndef SYM_SAVE_BUMP_SIZE
#define SYM_SAVE_BUMP_SIZE 4096
#endif

typedef void (*sym_save_oom_handler_t)(void *, const char *msg);

void sym_save_init(void);
const char *sym_save(rb_node **root, const char *s, size_t n);

#endif
