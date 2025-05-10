#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "core_def.h"
#include "heap_list.h"
#include "safe_str.h"

#define KIND(name, repr) {.kind_name = name, .repr_fn = repr}

// reprs
char *null_repr(Node *self) {
  (void)self;
  return STR_LITERAL_DUP("NULL");
}

char *literal_integer_repr(Node *self) {
  char str[CLSR_INTEGER_TYPE_STR_MAX_SIZE];
  size_t n = sizeof(str);

  int result = snprintf(str, n, CLSR_INTEGER_TYPE_FMT, get_integer(self));

  if (result < 0 || (size_t)result >= n)
    return NULL;

  return safe_strndup(str, n);
}

char *literal_symbol_repr(Node *self) {
  const char *str = get_symbol(self);
  return safe_strndup(str, strlen(str));
}

char *list_repr(Node *self) {
  HeapList *hl = NULL;
  size_t total = 0;
  Node *cur;

  hl = hl_alloc();

  if (!hl)
    return NULL;

  total += hl_append_strdup(hl, "(");

  for (cur = self; is_list(cur); cur = get_cdr(cur)) {
    Node *car = get_car(cur), *cdr = get_cdr(cur);

    if (car) {
      total += hl_append_strdup(hl, get_kind(car)->repr_fn(car));

      if (get_car(cdr))
        total += hl_append_strdup(hl, " ");
    }
  }

  if (cur) {
    total += hl_append_strdup(hl, ".");
    total += hl_append_strdup(hl, get_kind(cur)->repr_fn(cur));
  }

  total += hl_append_strdup(hl, ")");

  // merge down into a single str

  char *repr_str = calloc(total + 1, sizeof *(repr_str));
  char *dst = repr_str;

  if (!repr_str)
    return NULL;

  for (size_t i = 0; i < hl->count; ++i) {
    for (char *src = hl->items[i]; (*dst = *src); ++src, ++dst)
      ;
    free(hl->items[i]);
  }

  hl_free(hl);

  return repr_str;
}

char *fn_prim_repr(Node *self) {
  const PrimOp *prim_op = get_prim_op(self);
  return safe_strndup(prim_op->name, strlen(prim_op->name));
}

char *fn_closure_repr(Node *self) {
  const char *fmt = "closure params=%s body=%s";

  char *params_str =
      get_kind(get_closure_params(self))->repr_fn(get_closure_params(self));
  char *body_str =
      get_kind(get_closure_body(self))->repr_fn(get_closure_body(self));

  size_t total = strlen(fmt) + SAFE_STRLEN(params_str) + SAFE_STRLEN(body_str);

  char *repr_str = calloc(total, sizeof *repr_str);
  if (!repr_str)
    return NULL;

  int result = snprintf(repr_str, total, fmt, params_str, body_str);
  if (result < 0 || (size_t)result >= total) {
    free(repr_str);
    return NULL;
  }

  free(params_str);
  free(body_str);

  return repr_str;
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
