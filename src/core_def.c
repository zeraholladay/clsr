#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#include "core_def.h"

#define KIND(name, repr) {.kind_name = name, .repr_fn = repr}

static int fmt_append(char *buf, size_t offset, const char *fmt, ...) {
  int n = 0;
  va_list args;
  va_start(args, fmt);
  n = vsnprintf(buf + offset, STR_FMT_BUF_SIZE - offset, fmt, args);
  va_end(args);
  return (n < 0 || (size_t)n >= STR_FMT_BUF_SIZE) ? offset : (offset + n);
}

// reprs
int null_repr(Node *self, char *buf, size_t offset) {
  (void)self;
  return fmt_append(buf, offset, "NULL");
}

int literal_integer_repr(Node *self, char *buf, size_t offset) {
  return fmt_append(buf, offset, "%d", get_integer(self));
}

int literal_symbol_repr(Node *self, char *buf, size_t offset) {
  return fmt_append(buf, offset, "%s", get_symbol(self));
}

int list_repr(Node *self, char *buf, size_t offset) {
  Node *cur = NULL;

  offset = fmt_append(buf, offset, "(");

  for (cur = self; is_list(cur); cur = get_cdr(cur)) {
    Node *car = get_car(cur), *cdr = get_cdr(cur);
    if (car) {
      offset = get_kind(car)->repr_fn(car, buf, offset);
      if (is_list(get_car(cdr))) {
        offset = fmt_append(buf, offset, " ");
      }
    }
  }
  if (cur) {
    offset = fmt_append(buf, offset, ".");
    offset = get_kind(cur)->repr_fn(cur, buf, offset);
  }
  offset = fmt_append(buf, offset, ")");

  return offset;
}

int fn_prim_repr(Node *self, char *buf, size_t offset) {
  const PrimOp *prim_op = get_prim_op(self);
  return fmt_append(buf, offset, "%s", prim_op->name);
}

int fn_closure_repr(Node *self, char *buf, size_t offset) {
  Node *params = get_closure_params(self);
  Node *body = get_closure_body(self);
  offset = fmt_append(buf, offset, "(closure ");
  offset = get_kind(params)->repr_fn(params, buf, offset);
  offset = get_kind(body)->repr_fn(body, buf, offset);
  return offset;
}

// kind singletons
static Kind null_kind_singleton = KIND("NULL", null_repr);

static Kind literal_kind_singleton[] = {
    [LITERAL_INTEGER] = KIND("literal.integer", literal_integer_repr),
    [LITERAL_SYMBOL] = KIND("literal.symbol", literal_symbol_repr),
};

static Kind list_kind_singleton[] = {
    [0] = KIND("list", list_repr),
};

static Kind fn_kind_singleton[] = {
    [FN_PRIMITIVE] = KIND("function.primitive", fn_prim_repr),
    [FN_CLOSURE] = KIND("function.closure", fn_closure_repr),
};

static Kind *kind_singleton[] = {
    [KIND_LITERAL] = literal_kind_singleton,
    [KIND_LIST] = list_kind_singleton,
    [KIND_FUNCTION] = fn_kind_singleton,
};

// get_kind
const Kind *get_kind(Node *self) {
  if (!self) {
    return &null_kind_singleton;
  }

  Kind *kind_ptr = kind_singleton[self->kind];

  if (is_literal(self)) {
    const Literal *literal = get_literal(self);
    return &kind_ptr[literal->kind];
  }

  if (is_function(self)) {
    const Function *fn = get_function(self);
    return &kind_ptr[fn->kind];
  }

  if (is_list(self)) {
    return &kind_ptr[0];
  }

  return NULL; // TODO: fix me
}

// constructors
Node *cons_primop(Pool *p, const PrimOp *prim_op) {
  Node *node = pool_alloc(p);
  node->kind = KIND_FUNCTION;
  Function *func = &node->as.function;
  func->kind = FN_PRIMITIVE;
  func->as.primitive.prim_op = prim_op;
  return node;
}

Node *cons_closure(Pool *p, Node *params, Node *body, Env *env) {
  Node *node = pool_alloc(p);
  node->kind = KIND_FUNCTION;
  Function *func = &node->as.function;
  func->kind = FN_CLOSURE;
  func->as.closure.params = params;
  func->as.closure.body = body;
  func->as.closure.env = env;
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
