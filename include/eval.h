#ifndef EVAL_H
#define EVAL_H

#include "core_def.h"

#define FIRST(node) (is_list(node) ? node->as.list.first : NULL)
#define CONS(first, rest, ctx) (cons_list(CTX_POOL(ctx), first, rest))
#define REST(node) (is_list(node) ? node->as.list.rest : NULL)

Node *eval_apply(Node *args, Context *ctx);
Node *eval_closure(Node *args, Context *ctx);
Node *eval_cons(Node *args, Context *ctx);
Node *eval_eq(Node *args, Context *ctx);
Node *eval_first(Node *node, Context *ctx);
Node *eval_if(Node *args, Context *ctx);
Node *eval_len(Node *args, Context *ctx);
Node *eval_pair(Node *args, Context *ctx);
Node *eval_print(Node *node, Context *ctx);
Node *eval_rest(Node *node, Context *ctx);
Node *eval_set(Node *args, Context *ctx);
Node *eval_str(Node *node, Context *ctx);
Node *eval(Node *expr, Context *ctx);
Node *eval_list(Node *args, Context *ctx);
Node *eval_program(Node *program, Context *ctx);

#endif
