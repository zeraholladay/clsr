#include "stack.h"
#include "common.h" // die

#define STACK_CAPACITY 256

static struct stack {
  uintptr_t sp;
  uintptr_t fp;
  uintptr_t data[STACK_CAPACITY];
} _stack = {
    .sp = 0,
    .fp = 0,
    .data = {0},
};

struct stack *s_ptr = &_stack;

void stack_reset() { s_ptr->sp = s_ptr->fp = 0; }

void push(void *value) {
  if (s_ptr->sp >= STACK_CAPACITY) { // TODO: grow
    die("Stack overflow!\n");
  }
  s_ptr->data[s_ptr->sp++] = (uintptr_t)value;
}

void *pop() {
  if (s_ptr->sp <= 0)
    return NULL;
  return (void *)s_ptr->data[--s_ptr->sp];
}

void *peek() {
  if (s_ptr->sp <= 0)
    return NULL;
  return (void *)(s_ptr->data[s_ptr->sp - 1]);
}

void enter_frame() {
  push((void *)s_ptr->fp);
  s_ptr->fp = s_ptr->sp;
}

void exit_frame() {
  s_ptr->sp = s_ptr->fp;
  s_ptr->fp = (uintptr_t)pop();
}
