#ifndef CLSR_H
#define CLSR_H

// #include "common.h"
// #include "env.h"
// #include "palloc.h"
// #include "rb_tree.h"
// #include "stack.h"

// /* primitive operations */

// extern struct Node *const true;
// extern struct Node *const false;

// struct Node;
// struct Context;

// typedef struct Node *(*PrimFunc)(struct Node *Node, struct Context *ctx);

// typedef struct PrimOp {
//   int tok;
//   PrimFunc prim_fun;
// } PrimOp;

// typedef enum { KIND_LITERAL, KIND_LIST, KIND_FUNCTION } Kind;

// typedef enum { LITERAL_INTEGER, LITERAL_KEYWORD, LITERAL_SYMBOL } LiteralKind;

// typedef struct {
//   LiteralKind kind;
//   union {
//     int integer;
//     const char *symbol;
//   } as;
// } Literal;

// typedef struct Node Node;

// typedef struct {
//   size_t count;
//   size_t capacity;
//   Node **items;
// } List;

// typedef enum { FN_PRIMITIVE, FN_CLOSURE } FnKind;

// typedef struct {
//   FnKind kind;
//   union {
//     struct {
//       PrimFunc *fn_ptr;
//     } primitive;

//     struct {
//       Node *params;
//       Node *body;
//       Env *env;
//     } closure;
//   } as;
// } Function;

// struct Node {
//   Kind kind;
//   union {
//     List *list;
//     Literal literal;
//     Function function;
//   } as;
// };

// /* clsr context */

// #define CTX_POOL(ctx) ((ctx)->Node_pool)
// #define CTX_ENV(ctx) ((ctx)->eval_ctx.env)
// #define CTX_STACK(ctx) ((ctx)->eval_ctx.stack)
// #define CTX_SYMTAB(ctx) ((ctx)->parser_ctx.sym_tab)
// #define CTX_PARSE_ROOT(ctx) ((ctx)->parser_ctx.root_Node)
// #define CTX_PARSE_MARK(ctx) ((ctx)->parser_ctx.parse_mark)
// #define CTX_PEEK(ctx) PEEK(CTX_STACK(ctx))
// #define CTX_POP(ctx) POP(CTX_STACK(ctx))
// #define CTX_PUSH(ctx, Node) PUSH(CTX_STACK(ctx), Node)

// typedef struct EvalContext {
//   Env *env;
//   Stack *stack;
// } EvalContext;

// typedef struct ParserContext {
//   rb_node *sym_tab;
//   Node *root_Node;
//   Wrapper *parse_mark;
// } ParserContext;

// typedef struct Context {
//   Pool *Node_pool;
//   EvalContext eval_ctx;
//   ParserContext parser_ctx;
// } Context;

// /* prim_op.gperf */
// const PrimOp *prim_op_lookup(register const char *str,
//                              register unsigned int len);

// /* stack.c (stack helpers) */

// #define STACK_INIT(stack) stack_init(stack, STACK_GROWTH)
// #define STACK_FREE(stack) stack_free(stack)
// #define PUSH(stack, Node) stack_push(stack, (void *)Node)
// #define POP(stack) (Node *)stack_pop(stack)
// #define PEEK(stack) (Node *)stack_peek(stack)
// #define ENTER_FRAME(stack) stack_enter_frame(stack)
// #define EXIT_FRAME(stack) stack_exit_frame(stack)

/* Node.c */

// Node *Node_new_integer(Pool *p, int i);
// Node *Node_new_literal_sym(Pool *p, const char *sym);
// Node *Node_new_empty_expr_list(Pool *p);
// Node *Node_expr_list_append(Node *Node, Node *item);
// Node *Node_new_call(Pool *p, const PrimOp *prim, Node *args);
// Node *Node_new_closure(Pool *p, Node *params, Node *body);
// Node *Node_new_if(Pool *p, Node *then, Node *else_);
// void Node_fprintf(FILE *restrict stream, const Node *Node);

/* list.c */

// List *list_new(void);
// int list_append(List *Node_list, Node *Node);

/* eval.c */

// Node *apply(Node *void_Node, Context *ctx);
// Node *closure(Node *Node, Context *ctx);
// Node *eq(Node *Node, Context *ctx);
// Node *if_(Node *Node, Context *ctx);
// Node *is(Node *Node, Context *ctx);
// Node *lookup(Node *void_Node, Context *ctx);
// Node *push(Node *Node, Context *ctx);
// Node *quote(Node *Node, Context *void_ctx);
// Node *return_(Node *void_Node, Context *ctx);
// Node *set(Node *void_Node, Context *ctx);
// Node *eval(Node *Node, Context *ctx);
// Node *eval_list(Node *Node, Context *ctx);
// Node *eval_program(Node *program, Context *ctx);

/* math.c */

// Node *add(Node *void_Node, Context *ctx);
// Node *sub(Node *void_Node, Context *ctx);
// Node *mul(Node *void_Node, Context *ctx);
// Node *div_(Node *void_Node, Context *ctx);

#endif
