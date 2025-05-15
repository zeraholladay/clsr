#ifndef EVAL_H
#define EVAL_H

#include "eval_ctx.h"
#include "types.h"

#define FIRST(node) ((node)->as.list.first)
#define REST(node) ((node)->as.list.rest)

#define CONS(first, rest, ctx) (cons_list (CTX_POOL (ctx), first, rest))
#define EMPTY_LIST(ctx) (CONS (NULL, NULL, ctx))
#define LIST1(item, ctx) (CONS (item, EMPTY_LIST (ctx), ctx))
#define LIST2(first, rest, ctx) (CONS (first, LIST1 (rest, ctx), ctx))

#define PRINT(node, ctx) eval_print (node, ctx)

#define IS_EMPTY_LIST(node)                                                   \
  (!node || (IS_NIL (node))                                                   \
   || (IS_LIST (node) && !FIRST (node) && !REST (node)))

Node *eval_apply (Node *args, Context *ctx);
Node *eval_cons (Node *args, Context *ctx);
Node *eval_eq (Node *args, Context *ctx);
Node *eval_first (Node *args, Context *ctx);
Node *eval_funcall (Node *args, Context *ctx);
Node *eval_if (Node *args, Context *ctx);
Node *eval_len (Node *args, Context *ctx);
Node *eval_list (Node *args, Context *ctx);
Node *eval_pair (Node *args, Context *ctx);
Node *eval_print (Node *args, Context *ctx);
Node *eval_rest (Node *args, Context *ctx);
Node *eval_set (Node *args, Context *ctx);
Node *eval_str (Node *args, Context *ctx);
Node *eval (Node *expr, Context *ctx);
Node *eval_list (Node *args, Context *ctx);
Node *eval_program (Node *program, Context *ctx);

#endif
