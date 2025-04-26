#ifndef STACK_H
#define STACK_H

#include "obj.h"

#define STACK_GROWTH 4096

typedef struct stack {
  uintptr_t sp;
  uintptr_t fp;
  unsigned int data_size;
  uintptr_t *data;
} Stack;

#define STACK_INIT(stack) stack_init(stack, STACK_GROWTH)
#define PUSH(stack, obj) push(stack, (void *)obj)
#define POP(stack) (Obj *)pop(stack)
#define PEEK(stack) (Obj *)peek(stack)
#define ENTER_FRAME(stack) enter_frame(stack)
#define EXIT_FRAME(stack) exit_frame(stack)

void stack_init(Stack *s_ptr, unsigned int count);
void stack_free(Stack *s_ptr);
void push(Stack *s_ptr, void *value);
void *pop(Stack *s_ptr);
void *peek(Stack *s_ptr);
void enter_frame(Stack *s_ptr);
void exit_frame(Stack *s_ptr);

#endif