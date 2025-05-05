#include <stdio.h>
#include <stdlib.h>

#include "node_alloc.h"

Node *alloc_integer(Pool *p, int i) {
  Node *node = pool_alloc(p);
  node->kind = KIND_LITERAL;
  Literal literal = node->as.literal;
  literal.kind = LITERAL_INTEGER;
  literal.as.integer = i;
  return node;
}

Node *alloc_symbol(Pool *p, const char *sym) {
  Node *node = pool_alloc(p);
  node->kind = KIND_LITERAL;
  Literal literal = node->as.literal;
  literal.kind = LITERAL_SYMBOL;
  literal.as.symbol = sym;
  return node;
}

Node *alloc_prim_func(Pool *p, const PrimOp *prim) {
  Node *node = pool_alloc(p);
  node->kind = KIND_FUNCTION;
  Function func = node->as.function;
  func.kind = FN_PRIMITIVE;
  func.as.primitive.fn_ptr = prim->prim_fun;
  return node;
}

Node *alloc_closure(Pool *p, Node *params, Node *body) {
  Node *node = pool_alloc(p);
  node->kind = KIND_FUNCTION;
  Function func = node->as.function;
  func.kind = FN_CLOSURE;
  func.as.closure.params = params;
  func.as.closure.body = body;
  func.as.closure.env = NULL;
  return node;
}
