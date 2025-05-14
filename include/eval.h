#ifndef EVAL_H
#define EVAL_H

#include "core_def.h"
#include "eval_ctx.h"

#define FIRST(node)            ((node)->as.list.first)
#define CONS(first, rest, ctx) (cons_list(CTX_POOL(ctx), first, rest))
#define REST(node)             ((node)->as.list.rest)

#define IS_EMPTY_LIST(node)                                                    \
  (!node || (IS_NIL(node)) || (IS_LIST(node) && !FIRST(node) && !REST(node)))

Node *eval_apply(Node *args, Context *ctx);
Node *eval_closure(Node *args, Context *ctx);
Node *eval_cons(Node *args, Context *ctx);
Node *eval_eq(Node *args, Context *ctx);
Node *eval_first(Node *args, Context *ctx);
Node *eval_if(Node *args, Context *ctx);
Node *eval_len(Node *args, Context *ctx);
Node *eval_pair(Node *args, Context *ctx);
Node *eval_print(Node *args, Context *ctx);
Node *eval_rest(Node *args, Context *ctx);
Node *eval_set(Node *args, Context *ctx);
Node *eval_str(Node *args, Context *ctx);
Node *eval(Node *expr, Context *ctx);
Node *eval_list(Node *args, Context *ctx);
Node *eval_program(Node *program, Context *ctx);

#endif
