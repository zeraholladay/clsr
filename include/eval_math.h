#ifndef EVAL_MATH_H
#define EVAL_MATH_H

#include "eval.h"

Node *eval_add (Node *args, Context *ctx);
Node *eval_sub (Node *args, Context *ctx);
Node *eval_mul (Node *args, Context *ctx);
Node *eval_div (Node *args, Context *ctx);

#endif
