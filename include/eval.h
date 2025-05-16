#ifndef EVAL_H
#define EVAL_H

#include "eval_ctx.h"
#include "types.h"

extern Node t_node;
extern Node nil_node;

#define T (&t_node)
#define NIL (&nil_node)

#define IS_NIL(node) (node == NIL)
#define LISTP(node) (IS_NIL (node) || IS_LIST (node))

#define FIRST(node) ((node)->as.list.first)
#define REST(node) ((node)->as.list.rest)

#define CONS(first, rest, ctx) (cons_list (&CTX_POOL (ctx), first, rest))
#define LIST1(item, ctx) (CONS (item, NIL, ctx))
#define LIST2(first, rest, ctx) (CONS (first, LIST1 (rest, ctx), ctx))

Node *eval_apply (Node *args, Context *ctx);
Node *eval_cons (Node *args, Context *ctx);
Node *eval_eq (Node *args, Context *ctx);
Node *eval_first (Node *args, Context *ctx);
Node *eval_funcall (Node *args, Context *ctx);
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
