#ifndef CORE_DEF_H
#define CORE_DEF_H

#include <limits.h>
#include <stddef.h>

#include "env.h"
#include "palloc.h"
#include "rb_tree.h"
#include "stack.h"

// Primitive macros
#define PRIMITIVE(name) primitive_lookup(#name, sizeof(#name) - 1)

// formatting
#ifndef CLSR_INTEGER_TYPE
#define CLSR_INTEGER_TYPE long long
#define CLSR_INTEGER_TYPE_FMT "%lld"
#endif

#define LOG10_2 0.30103
#define CLSR_INTEGER_TYPE_STR_MAX_SIZE                                         \
  ((size_t)(sizeof(CLSR_INTEGER_TYPE) * CHAR_BIT * LOG10_2 + 3))

// Context macros
#define CTX_POOL(ctx) ((ctx)->node_pool)
#define CTX_ENV(ctx) ((ctx)->eval_ctx.env)
#define CTX_SYMTAB(ctx) ((ctx)->parser_ctx.sym_tab)
#define CTX_PARSE_ROOT(ctx) ((ctx)->parser_ctx.root_node)
#define CTX_PARSE_MARK(ctx) ((ctx)->parser_ctx.parse_mark)

// Forward declarations
struct Node;
struct Context;
typedef struct Node Node;
typedef struct Context Context;

// Primitive

typedef Node *(*prim_fn_t)(Node *, Context *);

typedef struct Primitive {
  const char *name;
  prim_fn_t fn;
  int arity;
} Primitive;

// Node type "object"
typedef char *(*StrFn)(Node *);
typedef int (*EqFn)(Node *, Node *);

typedef struct Type {
  const char *type_name;
  StrFn str_fn;
  EqFn eq_fn;
} Type;

// Nodes
typedef enum {
  TYPE_NIL,       // special constant
  TYPE_SYMBOL,    // identifiers
  TYPE_INTEGER,   // literal
  TYPE_STRING,    // literal
  TYPE_LIST,      // cons cells
  TYPE_PRIMITIVE, // builtin fn
  TYPE_CLOSURE    // user-defined fn
} TypeEnum;

typedef struct {
  Node *first;
  Node *rest;
} List;

typedef struct {
  Node *params;
  Node *body;
  Env *env;
} Closure;

struct Node {
  TypeEnum type;
  union {
    // Literal values
    CLSR_INTEGER_TYPE integer;
    char *string;
    const char *symbol;

    // Composite structures
    List list;

    // Function-like values
    const Primitive *primitive;
    Closure closure;
  } as;
};

typedef struct EvalContext {
  Env *env;
} EvalContext;

typedef struct ParserContext {
  rb_node *sym_tab;
  Node *root_node;
  Wrapper *parse_mark;
} ParserContext;

struct Context {
  Pool *node_pool;
  EvalContext eval_ctx;
  ParserContext parser_ctx;
};

// coredef.c
const Type *type(Node *self);
Node *cons_primop(Pool *p, const Primitive *prim_op);
Node *cons_closure(Pool *p, Node *params, Node *body, Env *env);
Node *cons_integer(Pool *p, CLSR_INTEGER_TYPE i);
Node *cons_list(Pool *p, Node *car, Node *cdr);
Node *cons_string(Pool *p, char *str);
Node *cons_symbol(Pool *p, const char *sym);

// helpers
static inline int is_nil(const Node *node) {
  return node && node->type == TYPE_NIL;
}

static inline int is_list(const Node *node) {
  return node && node->type == TYPE_LIST;
}

static inline int is_string(const Node *node) {
  return node && node->type == TYPE_STRING;
}

// Literal type checks
static inline int is_integer(const Node *node) {
  return node && node->type == TYPE_INTEGER;
}

static inline int is_symbol(const Node *node) {
  return node && node->type == TYPE_SYMBOL;
}

// Function type checks
static inline int is_primitive_fn(const Node *node) {
  return node && node->type == TYPE_PRIMITIVE;
}

static inline int is_closure_fn(const Node *node) {
  return node && node->type == TYPE_CLOSURE;
}

static inline int is_empty_list(const Node *node) {
  return !node ||
         (is_list(node) && !node->as.list.first && !node->as.list.rest);
}

static inline CLSR_INTEGER_TYPE get_integer(Node *node) {
  return is_integer(node) ? node->as.integer : 0;
}

static inline const char *get_symbol(Node *node) {
  return is_symbol(node) ? node->as.symbol : NULL;
}

static inline const Primitive *get_prim_op(Node *node) {
  return is_primitive_fn(node) ? node->as.primitive : NULL;
}

static inline Closure *get_closure(Node *node) {
  return is_closure_fn(node) ? &node->as.closure : NULL;
}

static inline Node *get_closure_params(Node *node) {
  return is_closure_fn(node) ? node->as.closure.params : NULL;
}

static inline Node *get_closure_body(Node *node) {
  return is_closure_fn(node) ? node->as.closure.body : NULL;
}

static inline Env *get_closure_env(Node *node) {
  return is_closure_fn(node) ? node->as.closure.env : NULL;
}

// String accessors
static inline char *get_string(Node *node) {
  return is_string(node) ? node->as.string : NULL;
}

/* prim_op.gperf */
const struct Primitive *primitive_lookup(register const char *str,
                                         register size_t len);

#endif
