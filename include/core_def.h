#ifndef CORE_DEF_H
#define CORE_DEF_H

#include <stddef.h>

#include "env.h"
#include "palloc.h"
#include "rb_tree.h"
#include "stack.h"

#ifndef STR_FMT_BUF_SIZE
#define STR_FMT_BUF_SIZE 1024
#endif

#define PRIM_OP(name) prim_op_lookup(#name, sizeof(#name) - 1)

#define CTX_POOL(ctx) ((ctx)->node_pool)
#define CTX_ENV(ctx) ((ctx)->eval_ctx.env)
#define CTX_STACK(ctx) ((ctx)->eval_ctx.stack)
#define CTX_SYMTAB(ctx) ((ctx)->parser_ctx.sym_tab)
#define CTX_PARSE_ROOT(ctx) ((ctx)->parser_ctx.root_Node)
#define CTX_PARSE_MARK(ctx) ((ctx)->parser_ctx.parse_mark)
#define CTX_PEEK(ctx) PEEK(CTX_STACK(ctx))
#define CTX_POP(ctx) POP(CTX_STACK(ctx))
#define CTX_PUSH(ctx, Node) PUSH(CTX_STACK(ctx), Node)

// Forward declarations
struct Node;
struct Context;

// PrimOp
typedef struct Node *(*PrimUnaryFunc)(struct Node *node, struct Context *ctx);
typedef struct Node *(*PrimBinaryFunc)(struct Node *node1, struct Node *node2,
                                       struct Context *ctx);

typedef struct PrimOp {
  const char *name;
  const int tok;
  const PrimUnaryFunc unary_f_ptr;
  const PrimBinaryFunc binary_f_ptr;
} PrimOp;

// Node kind "object"
typedef int (*ReprFn)(struct Node *self, char *buf, size_t offset);

typedef struct Kind {
  const char *kind_name;
  ReprFn repr_fn;
} Kind;

// Nodes
typedef enum { KIND_LITERAL, KIND_LIST, KIND_FUNCTION } KindEnum;

typedef enum { LITERAL_INTEGER, LITERAL_SYMBOL } LiteralKindEnum;

typedef struct {
  LiteralKindEnum kind;
  union {
    int integer;
    const char *symbol;
  } as;
} Literal;

typedef struct Node Node;

typedef struct {
  Node *car;
  Node *cdr;
} List;

typedef enum { FN_PRIMITIVE, FN_CLOSURE } FnKindEnum;

typedef struct {
  FnKindEnum kind;
  union {
    struct {
      const PrimOp *prim_op;
    } primitive;
    struct {
      Node *params;
      Node *body;
      Env *env;
    } closure;
  } as;
} Function;

struct Node {
  KindEnum kind;
  union {
    List list;
    Literal literal;
    Function function;
  } as;
};

typedef struct EvalContext {
  Env *env;
  Stack *stack;
} EvalContext;

typedef struct ParserContext {
  rb_node *sym_tab;
  Node *root_Node;
  Wrapper *parse_mark;
} ParserContext;

typedef struct Context {
  Pool *node_pool;
  EvalContext eval_ctx;
  ParserContext parser_ctx;
} Context;

/* prim_op.gperf */
const struct PrimOp *prim_op_lookup(register const char *str,
                                    register size_t len);

// coredef.c
const Kind *get_kind(Node *self);
Node *cons_primop(Pool *p, const PrimOp *prim_op);
Node *cons_closure(Pool *p, Node *params, Node *body, Env *env);
Node *cons_integer(Pool *p, int i);
Node *cons_list(Pool *p, Node *car, Node *cdr);
Node *cons_symbol(Pool *p, const char *sym);

// helpers
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
static inline Literal *get_literal(Node *node) {
  return is_literal(node) ? &node->as.literal : NULL;
}

static inline int get_integer(Node *node) {
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

/* tagging along for now. */
#include <stdio.h>

void node_fprintf(FILE *stream, const Node *node);

#endif
