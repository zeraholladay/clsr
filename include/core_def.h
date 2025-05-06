#ifndef CORE_DEF_H
#define CORE_DEF_H

#include <stddef.h>

#include "env.h"
#include "palloc.h"
#include "rb_tree.h"
#include "stack.h"

// Forward declarations
struct Node;
struct Context;

typedef struct Node *(*PrimUnaryFunc)(struct Node *n, struct Context *ctx);
typedef struct Node *(*PrimBinaryFunc)(struct Node *n1, struct Node *n2,
                                       struct Context *ctx);

typedef struct PrimOp {
  const int tok;
  const PrimUnaryFunc unary_f_ptr;
  const PrimBinaryFunc binary_f_ptr;
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
  Node *car;
  Node *cdr;
} List;

typedef enum { FN_PRIMITIVE, FN_CLOSURE } FnKind;

typedef struct {
  FnKind kind;
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
  Kind kind;
  union {
    List list;
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

/* core_def.c */
Node *cons_c_fn(Pool *p, const PrimOp *prim_op);
Node *cons_closure(Pool *p, Node *params, Node *body);
Node *cons_integer(Pool *p, int i);
Node *cons_list(Pool *p, Node *car, Node *cdr);
Node *cons_symbol(Pool *p, const char *sym);

/* prim_op.gperf */
#define PRIM_OP(name) prim_op_lookup(#name, sizeof(#name) - 1)

const PrimOp *prim_op_lookup(register const char *str,
                             register unsigned int len);

/* tagging along for now. */
#include <stdio.h>

void node_fprintf(FILE *stream, const Node *node);

#endif
