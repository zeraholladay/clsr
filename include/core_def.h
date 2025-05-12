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
/* PRIMITIVE_NULL for quote. */
typedef enum {
  PRIMITIVE_NULL,
  PRIMITIVE_UNARY_FN,
  PRIMITIVE_BINARY_FN
} PrimitiveEnum;

typedef Node *(*PrimitiveUnaryFn)(Node *node, Context *ctx);
typedef Node *(*PrimitiveBinaryFn)(Node *node1, Node *node2, Context *ctx);

typedef struct Primitive {
  const char *name;
  const PrimitiveEnum type;
  union {
    const PrimitiveUnaryFn unary_fn_ptr;
    const PrimitiveBinaryFn binary_fn_ptr;
  };
} Primitive;

// Node type "object"
typedef char *(*StrFn)(Node *self);
typedef int (*EqFn)(Node *self, Node *other);

typedef struct Type {
  const char *type_name;
  StrFn str_fn;
  EqFn eq_fn;
} Type;

// Nodes
typedef enum {
  TYPE_NULL,
  TYPE_LITERAL,
  TYPE_LIST,
  TYPE_FUNCTION,
  TYPE_STRING
} TypeEnum;

typedef enum { LITERAL_INTEGER, LITERAL_SYMBOL } LiteralTypeEnum;
typedef enum { FN_PRIMITIVE, FN_CLOSURE } FunctionTypeEnum;

typedef char String; // FIXME: Strings should have a len

typedef struct {
  LiteralTypeEnum type;
  union {
    CLSR_INTEGER_TYPE integer;
    const char *symbol;
  } as;
} Literal;

typedef struct {
  Node *car; // Contents of the Address part of Register
  Node *cdr; // Contents of the Decrement part of Register
} List;

typedef struct {
  Node *params;
  Node *body;
  Env *env;
} Closure;

typedef struct {
  FunctionTypeEnum type;
  union {
    struct {
      const Primitive *prim_op;
    } primitive;
    Closure closure;
  } as;
} Function;

struct Node {
  TypeEnum type;
  union {
    List list;
    Literal literal;
    Function function;
    String *string;
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
Node *cons_string(Pool *p, String *str);
Node *cons_symbol(Pool *p, const char *sym);

// helpers
static inline int is_literal(const Node *node) {
  return node && node->type == TYPE_LITERAL;
}

static inline int is_list(const Node *node) {
  return node && node->type == TYPE_LIST;
}

static inline int is_function(const Node *node) {
  return node && node->type == TYPE_FUNCTION;
}

static inline int is_string(const Node *node) {
  return node && node->type == TYPE_STRING;
}

// Literal type checks
static inline int is_integer(const Node *node) {
  return is_literal(node) && node->as.literal.type == LITERAL_INTEGER;
}

static inline int is_symbol(const Node *node) {
  return is_literal(node) && node->as.literal.type == LITERAL_SYMBOL;
}

// Function type checks
static inline int is_primitive_fn(const Node *node) {
  return is_function(node) && node->as.function.type == FN_PRIMITIVE;
}

static inline int is_closure_fn(const Node *node) {
  return is_function(node) && node->as.function.type == FN_CLOSURE;
}

static inline int is_empty_list(const Node *node) {
  return !node || (is_list(node) && !node->as.list.car && !node->as.list.cdr);
}

static inline Node *empty_list(Context *ctx) {
  return cons_list(CTX_POOL(ctx), NULL, NULL);
}

// Literal accessors
static inline Literal *get_literal(Node *node) {
  return is_literal(node) ? &node->as.literal : NULL;
}

static inline CLSR_INTEGER_TYPE get_integer(Node *node) {
  return is_integer(node) ? node->as.literal.as.integer : 0;
}

static inline const char *get_symbol(Node *node) {
  return is_symbol(node) ? node->as.literal.as.symbol : NULL;
}

// List accessors
static inline List *get_list(Node *node) {
  return is_list(node) ? &node->as.list : NULL;
}

static inline Node *get_car(Node *node) {
  return is_list(node) ? node->as.list.car : NULL;
}

static inline Node *get_cdr(Node *node) {
  return is_list(node) ? node->as.list.cdr : NULL;
}

// Function accessors
static inline Function *get_function(Node *node) {
  return is_function(node) ? &node->as.function : NULL;
}

static inline const Primitive *get_prim_op(Node *node) {
  return is_primitive_fn(node) ? node->as.function.as.primitive.prim_op : NULL;
}

static inline Closure *get_closure(Node *node) {
  return is_closure_fn(node) ? &node->as.function.as.closure : NULL;
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

// String accessors
static inline String *get_string(Node *node) {
  return is_string(node) ? node->as.string : NULL;
}

/* prim_op.gperf */
const struct Primitive *primitive_lookup(register const char *str,
                                         register size_t len);

#endif
