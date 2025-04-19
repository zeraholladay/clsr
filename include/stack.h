#ifndef STACK_H
#define STACK_H

#include <stdio.h>
#include <stdlib.h>

#ifndef STACK_SIZE
#define STACK_SIZE 1024
#endif

typedef struct stack {
  unsigned int sp; // stack pointer
  unsigned int fp; // frame pointer
  int stack[STACK_SIZE];
} Stack;

#define ENTER_FRAME(s_ptr)                                                     \
  do {                                                                         \
    if ((s_ptr)->sp >= STACK_SIZE) {                                           \
      fprintf(stderr, "[STACK] Cannot enter frame: overflow\n");               \
      exit(EXIT_FAILURE);                                                      \
    }                                                                          \
    (s_ptr)->stack[(s_ptr)->sp++] = (s_ptr)->fp;                               \
    (s_ptr)->fp = (s_ptr)->sp;                                                 \
  } while (0)

#define EXIT_FRAME(s_ptr)                                                      \
  do {                                                                         \
    if ((s_ptr)->fp == 0 || (s_ptr)->fp > (s_ptr)->sp) {                       \
      fprintf(stderr, "[STACK] Invalid frame exit\n");                         \
      exit(EXIT_FAILURE);                                                      \
    }                                                                          \
    (s_ptr)->sp = (s_ptr)->fp;                                                 \
    (s_ptr)->fp = (s_ptr)->stack[--(s_ptr)->fp];                               \
  } while (0)

#define PUSH(s_ptr, val)                                                       \
  do {                                                                         \
    if ((s_ptr)->sp >= STACK_SIZE) {                                           \
      fprintf(stderr, "[STACK] Overflow\n");                                   \
      exit(EXIT_FAILURE);                                                      \
    }                                                                          \
    (s_ptr)->stack[(s_ptr)->sp++] = (val);                                     \
  } while (0)

#define POP(s, out)                                                            \
  do {                                                                         \
    if ((s_ptr)->sp == 0) {                                                    \
      fprintf(stderr, "[STACK] Underflow\n");                                  \
      exit(EXIT_FAILURE);                                                      \
    }                                                                          \
    (out) = (s_ptr)->stack[--(s_ptr)->sp];                                     \
  } while (0)

// void push(int val);
// int pop();

#endif