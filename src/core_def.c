#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "core_def.h"
#include "heap_list.h"
#include "safe_str.h"

#define KIND(name, str, eq) {.type_name = name, .str_fn = str, .eq_fn = eq}

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

  for (cur = self; is_list(cur); cur = get_cdr(cur)) {
    Node *car = get_car(cur), *cdr = get_cdr(cur);

    if (car) {
      total += hl_append_strdup(hl, type(car)->str_fn(car));

      if (get_car(cdr))
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
         (0 == strcmp(get_string(self),
                      get_string(other))); // FIX ME: strings should have len
}

static char *string_str(Node *self) { return get_string(self); }

// Singletons
static Type nil_singleton[] = {
    [0] = KIND("NIL", nil_tostr, nil_eq),
};

static Type literal_singleton[] = {
    [LITERAL_INTEGER] = KIND("Integer", integer_tostr, integer_eq),
    [LITERAL_SYMBOL] = KIND("Symbol", symbol_tostr, symbol_eq),
};

static Type list_singleton[] = {
    [0] = KIND("List", list_tostr, list_eq),
};

static Type str_singleton[] = {
    [0] = KIND("String", string_str, string_eq),
};

static Type fn_singleton[] = {
    [FN_PRIMITIVE] = KIND("Primitive", prim_op_tostr, prim_op_eq),
    [FN_CLOSURE] = KIND("Closure", closure_tostr, closure_eq),
};

static Type *type_singleton[] = {
    [TYPE_NIL] = nil_singleton,    [TYPE_LITERAL] = literal_singleton,
    [TYPE_LIST] = list_singleton,  [TYPE_FUNCTION] = fn_singleton,
    [TYPE_STRING] = str_singleton,
};

// type()
const Type *type(Node *self) {
  if (!self || is_nil(self)) {
    return &type_singleton[TYPE_NIL][0];
  }

  Type *type_ptr = type_ptr = type_singleton[self->type];

  if (is_literal(self)) {
    const Literal *literal = get_literal(self);
    return &type_ptr[literal->type];
  }

  if (is_function(self)) {
    const Function *fn = get_function(self);
    return &type_ptr[fn->type];
  }

  if (is_list(self)) {
    return &type_ptr[0];
  }

  if (is_string(self)) {
    return &type_ptr[0];
  }

  return NULL; // TODO: fix me
}

Node *cons_primop(Pool *p, const Primitive *prim_op) {
  Node *node = pool_alloc(p);
  node->type = TYPE_FUNCTION;
  Function *func = &node->as.function;
  func->type = FN_PRIMITIVE;
  func->as.primitive.prim_op = prim_op;
  return node;
}

Node *cons_closure(Pool *p, Node *params, Node *body, Env *env) {
  Node *node = pool_alloc(p);
  node->type = TYPE_FUNCTION;
  Function *func = &node->as.function;
  func->type = FN_CLOSURE;
  func->as.closure.params = params;
  func->as.closure.body = body;
  func->as.closure.env = env;
  return node;
}

Node *cons_integer(Pool *p, CLSR_INTEGER_TYPE i) {
  Node *node = pool_alloc(p);
  node->type = TYPE_LITERAL;
  Literal *literal = &node->as.literal;
  literal->type = LITERAL_INTEGER;
  literal->as.integer = i;
  return node;
}

Node *cons_list(Pool *p, Node *car, Node *cdr) {
  Node *node = pool_alloc(p);
  node->type = TYPE_LIST;
  List *list = &node->as.list;
  list->car = car;
  list->cdr = cdr;
  return node;
}

Node *cons_string(Pool *p, String *str) {
  Node *node = pool_alloc(p);
  node->type = TYPE_STRING;
  node->as.string = str;
  return node;
}

Node *cons_symbol(Pool *p, const char *sym) {
  Node *node = pool_alloc(p);
  node->type = TYPE_LITERAL;
  Literal *literal = &node->as.literal;
  literal->type = LITERAL_SYMBOL;
  literal->as.symbol = sym;
  return node;
}
