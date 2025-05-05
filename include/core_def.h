#ifndef CLSR_H
#define CLSR_H

#include <stddef.h>

#include "env.h"
#include "palloc.h"
#include "rb_tree.h"
#include "stack.h"

struct Node;
struct Context;

typedef struct Node *(*PrimFunc)(struct Node *Node, struct Context *ctx);

typedef struct PrimOp {
  int tok;
  PrimFunc prim_fun;
} PrimOp;

typedef enum {
  KIND_LITERAL,
  KIND_LIST,
  KIND_FUNCTION
} Kind;

typedef enum {
  LITERAL_INTEGER,
  LITERAL_KEYWORD,
  LITERAL_SYMBOL
} LiteralKind;

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

typedef enum {
  FN_PRIMITIVE,
  FN_CLOSURE
} FnKind;

typedef struct {
  FnKind kind;
  union {
    struct {
      PrimFunc *fn_ptr;
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

#define CTX_POOL(ctx) ((ctx)->Node_pool)
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
  Pool *Node_pool;
  EvalContext eval_ctx;
  ParserContext parser_ctx;
} Context;

static inline bool is_literal(const Node *node) {
  return node && node->kind == KIND_LITERAL;
}

static inline bool is_list(const Node *node) {
  return node && node->kind == KIND_LIST;
}

static inline bool is_function(const Node *node) {
  return node && node->kind == KIND_FUNCTION;
}

// Literal kind checks
static inline bool is_integer(const Node *node) {
  return is_literal(node) && node->as.literal.kind == LITERAL_INTEGER;
}

static inline bool is_keyword(const Node *node) {
  return is_literal(node) && node->as.literal.kind == LITERAL_KEYWORD;
}

static inline bool is_symbol(const Node *node) {
  return is_literal(node) && node->as.literal.kind == LITERAL_SYMBOL;
}

// Function kind checks
static inline bool is_primitive_fn(const Node *node) {
  return is_function(node) && node->as.function.kind == FN_PRIMITIVE;
}

static inline bool is_closure_fn(const Node *node) {
  return is_function(node) && node->as.function.kind == FN_CLOSURE;
}

#endif
