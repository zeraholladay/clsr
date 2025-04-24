#ifndef EVAL_H
#define EVAL_H

#include "env.h"
#include "obj.h"
#include "stack.h"

typedef struct EvalContext {
  // Stack *stack;
  Env *environ;
} EvalContext;

Obj *eval(Obj *obj, EvalContext *ctx);

#endif