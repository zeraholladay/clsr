#ifndef EVAL_H
#define EVAL_H

#include "env.h"
#include "stack.h"

typedef struct EvalContext {
  Stack *stack;
  Env *environ;
} EvalContext;

#endif