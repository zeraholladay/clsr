#include "log.h"

#ifndef STACK_SIZE
#define STACK_SIZE 1024
#endif

int stack[STACK_SIZE];
int sp = 0; // stack pointer
int fp = 0; // frame pointer

void push(int val) {
  if (sp >= STACK_SIZE) {
    ERRMSG("Stack overflow");
    return;
  }
  stack[sp++] = val;
}

int pop() {
  if (sp <= 0) {
    ERRMSG("Stack underflow\n");
    return 0;
  }
  return stack[--sp];
}