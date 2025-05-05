#ifndef CLSR_H
#define CLSR_H

#include <stddef.h>

#include "env.h"
#include "palloc.h"
#include "rb_tree.h"
#include "stack.h"

// Forward declarations
struct Node;
struct Context;

typedef struct Node *(*PrimFunc)(struct Node *Node, struct Context *ctx);

typedef struct PrimOp {
  const int tok;
  const PrimFunc fn_ptr;
} PrimOp;

typedef enum { KIND_LITERAL, KIND_LIST, KIND_FUNCTION } Kind;

typedef enum { LITERAL_INTEGER, LITERAL_KEYWORD, LITERAL_SYMBOL } LiteralKind;

typedef struct {
  LiteralKind kind;
  union {
    int integer;
    const char *symbol;
  } as;
} Literal;

typedef struct Node Node;

typedef struct {
  size_t count;
  size_t capacity;
  Node **items;
} List;

typedef enum { FN_PRIMITIVE, FN_CLOSURE } FnKind;

typedef struct {
  FnKind kind;
  union {
    struct {
      PrimFunc fn_ptr;
    } primitive;

    struct {
      Node *params;
      Node *body;
      Env *env;
    } closure;
  } as;
} Function;

struct Node {
  Kind kind;
  union {
    List *list;
    Literal literal;
    Function function;
  } as;
};

#define CTX_POOL(ctx) ((ctx)->node_pool)
#define CTX_ENV(ctx) ((ctx)->eval_ctx.env)
#define CTX_STACK(ctx) ((ctx)->eval_ctx.stack)
#define CTX_SYMTAB(ctx) ((ctx)->parser_ctx.sym_tab)
#define CTX_PARSE_ROOT(ctx) ((ctx)->parser_ctx.root_Node)
#define CTX_PARSE_MARK(ctx) ((ctx)->parser_ctx.parse_mark)
#define CTX_PEEK(ctx) PEEK(CTX_STACK(ctx))
#define CTX_POP(ctx) POP(CTX_STACK(ctx))
#define CTX_PUSH(ctx, Node) PUSH(CTX_STACK(ctx), Node)

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

/* core_def.c */
Node *new_integer(Pool *p, int i);
Node *new_symbol(Pool *p, const char *sym);
Node *new_prim_func(Pool *p, const PrimFunc fn_ptr);
Node *new_closure(Pool *p, Node *params, Node *body);

/* prim_op.gperf */
const PrimOp *prim_op_lookup(register const char *str,
                             register unsigned int len);

#endif
