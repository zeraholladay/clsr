#include "core_def.h"

// NULL
static const char *null_repr(Node *node) {
  (void)node;
  return "NULL"; }

static const KindObject null_kind_object = {
    "NULL",
    null_repr,
};

// Literal
static const char *literal_integer_repr(Node *node) { return "NULL"; }

static const char *literal_symbol_repr(Node *node) { return get_symbol(node); }

static const KindObject literal_kind_objects[] = {[LITERAL_INTEGER] =
                                                      {
                                                          "literal.integer",
                                                          literal_integer_repr,
                                                      },
                                                  [LITERAL_SYMBOL] = {
                                                      "literal.symbol",
                                                      literal_symbol_repr,
                                                  }};

// List
static const char *list_repr(Node *node) { return get_symbol(node); }

static const KindObject list_kind_objects[] = {
    [0] =
        {
            "list",
            list_repr,
        },
};

// Functions
static const char *function_prim_repr(Node *node) { return ""; }

static const char *function_closure_repr(Node *node) { return ""; }

static const KindObject function_kind_objects[] = {[FN_PRIMITIVE] =
                                                       {
                                                           "function.primitive",
                                                           function_prim_repr,
                                                       },
                                                   [FN_CLOSURE] = {
                                                       "function.closure",
                                                       function_closure_repr,
                                                   }};

static const KindObject *kind_objects[] = {
    [KIND_LITERAL] = literal_kind_objects,
    [KIND_LIST] = list_kind_objects,
    [KIND_FUNCTION] = function_kind_objects,
};

// kind
const KindObject *kind(Node *node) {
  if (!node) {
    return &null_kind_object;
  }

  const KindObject *kind_ptr = kind_objects[node->kind];

  if (is_literal(node)) {
    Literal *literal = get_literal(node);
    return &kind_ptr[literal->kind];
  }

  if (is_function(node)) {
    Function *fn = get_function(node);
    return &kind_ptr[fn->kind];
  }

  if (is_list(node)) {
    return &kind_ptr[0];
  }

  return NULL; // TODO: fix me
}

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
