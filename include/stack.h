#ifndef STACK_H
#define STACK_H

#include <stdlib.h>

#ifndef STACK_GROWTH
#define STACK_GROWTH 4096
#endif

typedef struct stack {
  uintptr_t sp;
  uintptr_t fp;
  unsigned int data_size;
  uintptr_t *data;
} Stack;

void stack_init(Stack *s_ptr, unsigned int count);
void stack_free(Stack *s_ptr);
void stack_push(Stack *s_ptr, void *value);
void *stack_pop(Stack *s_ptr);
void *stack_peek(Stack *s_ptr);
void stack_enter_frame(Stack *s_ptr);
void stack_exit_frame(Stack *s_ptr);

#endif
