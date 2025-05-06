#include "core_def.h"

Node *cons_c_fn(Pool *p, const PrimOp *prim_op) {
  Node *node = pool_alloc(p);
  node->kind = KIND_FUNCTION;
  Function *func = &node->as.function;
  func->kind = FN_PRIMITIVE;
  func->as.primitive.prim_op = prim_op;
  return node;
}

Node *cons_closure(Pool *p, Node *params, Node *body) {
  Node *node = pool_alloc(p);
  node->kind = KIND_FUNCTION;
  Function *func = &node->as.function;
  func->kind = FN_CLOSURE;
  func->as.closure.params = params;
  func->as.closure.body = body;
  func->as.closure.env = NULL;
  return node;
}

Node *cons_integer(Pool *p, int i) {
  Node *node = pool_alloc(p);
  node->kind = KIND_LITERAL;
  Literal *literal = &node->as.literal;
  literal->kind = LITERAL_INTEGER;
  literal->as.integer = i;
  return node;
}

Node *cons_list(Pool *p, Node *car, Node *cdr) {
  Node *node = pool_alloc(p);
  node->kind = KIND_LIST;
  List *list = &node->as.list;
  list->car = car;
  list->cdr = cdr;
  return node;
}

Node *cons_symbol(Pool *p, const char *sym) {
  Node *node = pool_alloc(p);
  node->kind = KIND_LITERAL;
  Literal *literal = &node->as.literal;
  literal->kind = LITERAL_SYMBOL;
  literal->as.symbol = sym;
  return node;
}
