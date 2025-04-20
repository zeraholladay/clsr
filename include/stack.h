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
  int **stack;
} Stack;

#define ENTER_FRAME(s_ptr)                                                     \
  do {                                                                         \
    int **stack = (s_ptr)->stack;                                              \
    if ((s_ptr)->sp >= (s_ptr)->cur_size &&                                    \
        REALLOC_N(stack, (s_ptr)->cur_size) + STACK_GROWTH)                    \
      die("[STACK] ENTER_FRAME\n");                                            \
    (s_ptr)->stack[(s_ptr)->sp++] = (s_ptr)->fp;                               \
    (s_ptr)->fp = (s_ptr)->sp;                                                 \
  } while (0)

#define EXIT_FRAME(s_ptr)                                                      \
  do {                                                                         \
    if ((s_ptr)->fp == 0 || (s_ptr)->fp > (s_ptr)->sp)                         \
      die("[STACK] Invalid frame exit\n");                                     \
    (s_ptr)->sp = (s_ptr)->fp;                                                 \
    (s_ptr)->fp = (s_ptr)->stack[--(s_ptr)->fp];                               \
  } while (0)

#define PUSH(s_ptr, val)                                                       \
  do {                                                                         \
    if ((s_ptr)->sp >= (s_ptr)->cur_size &&                                    \
        REALLOC_N(stack, (s_ptr)->cur_size) + STACK_GROWTH)                    \
      die("[STACK] ENTER_FRAME\n");                                            \
    (s_ptr)->stack[(s_ptr)->sp++] = (val);                                     \
  } while (0)

#define POP(s, out)                                                            \
  do {                                                                         \
    if ((s_ptr)->sp == 0)                                                      \
      die("[STACK] Underflow\n");                                              \
    (out) = (s_ptr)->stack[--(s_ptr)->sp];                                     \
  } while (0)

#endif