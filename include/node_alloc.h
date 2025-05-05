#ifndef NODE_ALLOC_H
#define NODE_ALLOC_H

#include "core_def.h"

Node *alloc_integer(Pool *p, int i);
Node *alloc_symbol(Pool *p, const char *sym);
Node *alloc_prim_func(Pool *p, const PrimOp *prim);
Node *alloc_closure(Pool *p, Node *params, Node *body);

#endif
