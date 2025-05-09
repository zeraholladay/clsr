#ifndef EVAL_H
#define EVAL_H

#include "core_def.h"

static inline int is_literal(const Node *node) {
  return node && node->kind == KIND_LITERAL;
}

static inline int is_list(const Node *node) {
  return node && node->kind == KIND_LIST;
}

static inline int is_function(const Node *node) {
  return node && node->kind == KIND_FUNCTION;
}

// Literal kind checks
static inline int is_integer(const Node *node) {
  return is_literal(node) && node->as.literal.kind == LITERAL_INTEGER;
}

static inline int is_keyword(const Node *node) {
  return is_literal(node) && node->as.literal.kind == LITERAL_KEYWORD;
}

static inline int is_symbol(const Node *node) {
  return is_literal(node) && node->as.literal.kind == LITERAL_SYMBOL;
}

// Function kind checks
static inline int is_primitive_fn(const Node *node) {
  return is_function(node) && node->as.function.kind == FN_PRIMITIVE;
}

static inline int is_closure_fn(const Node *node) {
  return is_function(node) && node->as.function.kind == FN_CLOSURE;
}

static inline int is_empty_list(const Node *node) {
  return !node || (is_list(node) && !node->as.list.car && !node->as.list.cdr);
}

static inline Node *empty_list(Context *ctx) {
  return cons_list(CTX_POOL(ctx), NULL, NULL);
}

// Literal accessors
// static inline Literal *get_literal(Node *node) {
//   return is_literal(node) ? &node->as.literal : NULL;
// }

static inline int get_integer(Node *node) {
  return is_integer(node) ? node->as.literal.as.integer : 0;
}

static inline const char *get_symbol(Node *node) {
  return (is_symbol(node) || is_keyword(node)) ? node->as.literal.as.symbol
                                               : NULL;
}

// List accessors
// static inline List *get_list(Node *node) {
//   return is_list(node) ? &node->as.list : NULL;
// }

static inline Node *get_car(Node *node) {
  return is_list(node) ? node->as.list.car : NULL;
}

static inline Node *get_cdr(Node *node) {
  return is_list(node) ? node->as.list.cdr : NULL;
}

// Function accessors
// static inline Function *get_function(Node *node) {
//   return is_function(node) ? &node->as.function : NULL;
// }

static inline const PrimOp *get_prim_op(Node *node) {
  return is_primitive_fn(node) ? node->as.function.as.primitive.prim_op : NULL;
}

static inline Node *get_closure_params(Node *node) {
  return is_closure_fn(node) ? node->as.function.as.closure.params : NULL;
}

static inline Node *get_closure_body(Node *node) {
  return is_closure_fn(node) ? node->as.function.as.closure.body : NULL;
}

static inline Env *get_closure_env(Node *node) {
  return is_closure_fn(node) ? node->as.function.as.closure.env : NULL;
}

Node *apply(Node *node, Node *args, Context *ctx);
Node *closure(Node *params, Node *body, Context *ctx);
Node *cons(Node *car, Node *cdr, Context *ctx);
Node *first(Node *node, Context *ctx);
Node *length(Node *list, Context *ctx);
Node *lookup(Node *node, Context *ctx);
Node *pair(Node *list1, Node *list2, Context *ctx);
Node *quote(Node *node, Context *void_ctx);
Node *rest(Node *node, Context *ctx);
Node *set(Node *car, Node *cdr, Context *ctx);
Node *eval(Node *expr, Context *ctx);
Node *eval_list(Node *list, Context *ctx);
Node *eval_program(Node *program, Context *ctx);

#endif
