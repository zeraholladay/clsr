#ifndef EVAL_H
#define EVAL_H

#include "core_def.h"

Node *apply(Node *node, Node *args, Context *ctx);
Node *cons(Node *car, Node *cdr, Context *ctx);
Node *first(Node *node, Context *ctx);
Node *lookup(Node *node, Context *ctx);
Node *quote(Node *node, Context *void_ctx);
Node *rest(Node *node, Context *ctx);
Node *set(Node *car, Node *cdr, Context *ctx);
Node *eval(Node *expr, Context *ctx);
Node *eval_list(Node *list, Context *ctx);
Node *eval_program(Node *program, Context *ctx);

#endif
