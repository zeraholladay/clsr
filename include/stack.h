#ifndef STACK_H
#define STACK_H

#include "obj.h"

#define PUSH(o) push(o)
#define POP(o) (Obj *)pop()
#define PEEK(o) (Obj *)peek()
#define ENTER_FRAME() enter_frame()
#define EXIT_FRAME() exit_frame()

void stack_reset();
void push(void *value);
void *pop(void);
void *peek(void);
void enter_frame(void);
void exit_frame(void);

#endif