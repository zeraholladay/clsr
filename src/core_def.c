#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "core_def.h"
#include "eval.h"
#include "heap_list.h"
#include "safe_str.h"

// Type eq
static inline int type_eq(Node *self, Node *other) {
  return type(self) == type(other);
}

// NIL type
static int nil_eq(Node *self, Node *other) {
  (void)self;
  (void)other;
  return self == other;
}

static char *nil_tostr(Node *self) {
  (void)self;
  return STR_LITERAL_DUP("NIL");
}

// Integer type
static int integer_eq(Node *self, Node *other) {
  return type_eq(self, other) && get_integer(self) == get_integer(other);
}

static char *integer_tostr(Node *self) {
  char str[CLSR_INTEGER_TYPE_STR_MAX_SIZE];
  size_t n = sizeof(str);

  int result = snprintf(str, n, CLSR_INTEGER_TYPE_FMT, get_integer(self));

  if (result < 0 || (size_t)result >= n)
    return NULL;

  return safe_strndup(str, n);
}

// Symbol type
static int symbol_eq(Node *self, Node *other) {
  return type_eq(self, other) && get_symbol(self) == get_symbol(other);
}

static char *symbol_tostr(Node *self) {
  const char *str = get_symbol(self);
  return safe_strndup(str, strlen(str));
}

// List type
static int list_eq(Node *self, Node *other) {
  return type_eq(self, other) &&
         ((is_empty_list(self) && is_empty_list(other)) ||
          get_list(self) == get_list(other));
}

static char *list_tostr(Node *self) {
  HeapList *hl = NULL;
  size_t total = 0;
  Node *cur;

  hl = hl_alloc();

  if (!hl)
    return NULL;

  total += hl_append_strdup(hl, "(");

  for (cur = self; is_list(cur); cur = REST(cur)) {
    Node *car = FIRST(cur), *cdr = REST(cur);

    if (car) {
      total += hl_append_strdup(hl, type(car)->str_fn(car));

      if (FIRST(cdr))
        total += hl_append_strdup(hl, " ");
    }
  }

  if (cur) {
    total += hl_append_strdup(hl, ".");
    total += hl_append_strdup(hl, type(cur)->str_fn(cur));
  }

  total += hl_append_strdup(hl, ")");

  // merge down into a single str

  char *str = calloc(total + 1, sizeof *(str));
  char *dst = str;

  if (!str)
    return NULL;

  for (size_t i = 0; i < hl->count; ++i) {
    for (char *src = hl->items[i]; (*dst = *src); ++src, ++dst)
      ;
    free(hl->items[i]);
  }

  hl_free(hl);

  return str;
}

// Prim Ops type
static int prim_op_eq(Node *self, Node *other) {
  return type_eq(self, other) && get_prim_op(self) == get_prim_op(other);
}

static char *prim_op_tostr(Node *self) {
  const Primitive *prim_op = get_prim_op(self);
  return safe_strndup(prim_op->name, strlen(prim_op->name));
}

// Closure type
static int closure_eq(Node *self, Node *other) {
  return type_eq(self, other) && get_closure(self) == get_closure(other);
}

static char *closure_tostr(Node *self) {
  const char *fmt = "closure params=%s body=%s";

  char *params_str =
      type(get_closure_params(self))->str_fn(get_closure_params(self));
  char *body_str = type(get_closure_body(self))->str_fn(get_closure_body(self));

  size_t total =
      strlen(fmt) + NULLABLE_STRLEN(params_str) + NULLABLE_STRLEN(body_str);

  char *str = calloc(total, sizeof *str);
  if (!str)
    return NULL;

  int result = snprintf(str, total, fmt, params_str, body_str);
  if (result < 0 || (size_t)result >= total) {
    free(str);
    return NULL;
  }

  free(params_str);
  free(body_str);

  return str;
}

// String type
static int string_eq(Node *self, Node *other) {
  return type_eq(self, other) &&
         (!strcmp(get_string(self),
                  get_string(other))); // FIX ME: strings should have len
}

static char *string_tostr(Node *self) { return get_string(self); }

static Type type_singleton[] = {
    // Special constant
    [TYPE_NIL] = {.type_name = "NIL", .str_fn = nil_tostr, .eq_fn = nil_eq},

    // Literal values
    [TYPE_INTEGER] = {.type_name = "Integer",
                      .str_fn = integer_tostr,
                      .eq_fn = integer_eq},
    [TYPE_STRING] = {.type_name = "String",
                     .str_fn = string_tostr,
                     .eq_fn = string_eq},
    [TYPE_SYMBOL] = {.type_name = "Symbol",
                     .str_fn = symbol_tostr,
                     .eq_fn = symbol_eq},

    // Composite structures
    [TYPE_LIST] = {.type_name = "List", .str_fn = list_tostr, .eq_fn = list_eq},

    // Function-like values
    [TYPE_PRIMITIVE] = {.type_name = "Primitive",
                        .str_fn = prim_op_tostr,
                        .eq_fn = prim_op_eq},
    [TYPE_CLOSURE] = {.type_name = "Closure",
                      .str_fn = closure_tostr,
                      .eq_fn = closure_eq},
};

// type()
const Type *type(Node *self) {
  if (!self || is_nil(self))
    return &type_singleton[TYPE_NIL];
  return &type_singleton[self->type];
}

Node *cons_primop(Pool *p, const Primitive *prim_op) {
  Node *node = pool_alloc(p);
  node->type = TYPE_PRIMITIVE;
  node->as.primitive = prim_op;
  return node;
}

Node *cons_closure(Pool *p, Node *params, Node *body, Env *env) {
  Node *node = pool_alloc(p);
  node->type = TYPE_CLOSURE;
  node->as.closure.params = params;
  node->as.closure.body = body;
  node->as.closure.env = env;
  return node;
}

Node *cons_integer(Pool *p, CLSR_INTEGER_TYPE i) {
  Node *node = pool_alloc(p);
  node->type = TYPE_INTEGER;
  node->as.integer = i;
  return node;
}

Node *cons_list(Pool *p, Node *car, Node *cdr) {
  Node *node = pool_alloc(p);
  node->type = TYPE_LIST;
  node->as.list.first = car;
  node->as.list.rest = cdr;
  return node;
}

Node *cons_string(Pool *p, char *str) {
  Node *node = pool_alloc(p);
  node->type = TYPE_STRING;
  node->as.string = str;
  return node;
}

Node *cons_symbol(Pool *p, const char *sym) {
  Node *node = pool_alloc(p);
  node->type = TYPE_SYMBOL;
  node->as.symbol = sym;
  return node;
}
