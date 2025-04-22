// #include "stack.h"

// #include "common.h"

// #define STACK_INIT_CAPACITY 64
// #define STACK_GROWTH_FACTOR 2

// int stack_init(Stack *s, unsigned int capacity) {
//   s->sp = 0;
//   s->fp = 0;
//   s->cur_size = capacity;
//   s->stack = malloc(sizeof(Obj *) * capacity);
//   return s->stack != NULL;
// }

// void stack_free(Stack *s) {
//   free(s->stack);
//   s->stack = NULL;
//   s->sp = s->fp = s->cur_size = 0;
// }

// int stack_grow(Stack *s) {
//   unsigned int new_size = s->cur_size * STACK_GROWTH_FACTOR;
//   Obj **new_stack = realloc(s->stack, sizeof(Obj *) * new_size);
//   if (!new_stack)
//     return 0;
//   s->stack = new_stack;
//   s->cur_size = new_size;
//   return 1;
// }

// int stack_push(Stack *s, Obj *obj) {
//   if (s->sp >= s->cur_size) {
//     if (!stack_grow(s)) {
//       fprintf(stderr, "Stack overflow!\n");
//       return 0;
//     }
//   }
//   s->stack[s->sp++] = obj;
//   return 1;
// }

// Obj *stack_pop(Stack *s) {
//   if (s->sp == s->fp) {
//     fprintf(stderr, "Cannot pop: stack frame underflow\n");
//     return NULL;
//   }
//   return s->stack[--s->sp];
// }

// void stack_enter_frame(Stack *s) {
//   Obj *fp_marker = (Obj *)(uintptr_t)s->fp; // store fp as an integer-like
//   value stack_push(s, fp_marker); s->fp = s->sp;
// }

// void stack_exit_frame(Stack *s) {
//   if (s->fp == 0 || s->fp > s->sp) {
//     fprintf(stderr, "Invalid frame exit\n");
//     return;
//   }
//   // Pop down to the start of the current frame
//   while (s->sp > s->fp) {
//     stack_pop(s);
//   }
//   // Restore previous frame pointer
//   Obj *fp_marker = stack_pop(s);
//   s->fp = (unsigned int)(uintptr_t)fp_marker;
// }
