#ifndef EVAL_H
#define EVAL_H

#include "environ.h"
#include "stack.h"

typedef struct EvalContext {
  Stack *stack;
  Environ *environ;
} EvalContext;

#endif