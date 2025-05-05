#ifndef EVAL_H
#define EVAL_H

#include "core_def.h"

Node *quote(Node *node, Context *void_ctx);
Node *set(Node *args, Context *ctx);

Node *eval(Node *expr, Context *ctx);
Node *eval_list(Node *list, Context *ctx);
Node *eval_program(Node *program, Context *ctx);

#endif
