#ifndef SYM_SAVE
#define SYM_SAVE

#ifndef SYMTAB_COUNT
#define SYMTAB_COUNT 4096
#endif

#include <stdlib.h>

#include "rb_tree.h"

void sym_save_init(void);
const char *sym_save(rb_node **root, const char *s, size_t n);

#endif
