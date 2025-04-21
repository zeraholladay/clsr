#ifndef STACK_H
#define STACK_H

#include <stdio.h>
#include <stdlib.h>

#include "common.h"

#ifndef STACK_GROWTH
#define STACK_GROWTH 4096
#endif

typedef struct stack {
  unsigned int sp; // stack pointer
  unsigned int fp; // frame pointer
  unsigned int cur_size;
  Obj **stack;
} Stack;

#endif