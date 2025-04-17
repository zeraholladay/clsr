#include "mach.h"

#ifndef STACK_SIZE
#define STACK_SIZE 1024
#endif

int stack[STACK_SIZE];
int sp = 0; // stack pointer
int fp = 0; // frame pointer

void push(int val) {
  if (sp >= STACK_SIZE) {
    fprintf(stderr, "Stack overflow");
    return;
  }
  stack[sp++] = val;
}

int pop() {
  if (sp <= 0) {
    fprintf(stderr, "Stack underflow\n");
    return 0;
  }
  return stack[--sp];
}

#ifndef HEAP_SIZE
#define HEAP_SIZE 1024 * 4
#endif

object_t heap[HEAP_SIZE];
int heap_top = 0;

object_t *alloc_object(object_t_enum type, int *addr) {
  if (heap_top >= HEAP_SIZE) {
    DEBUG("[HEAP] Heap overflow\n"); // TO DO: fix me
    return 0;
  }
  *addr = heap_top;
  object_t *obj = &heap[heap_top++];
  obj->type = type;
  return obj;
}

void run_operator(const struct op *op_ptr) {
  int old_fp = fp;

  if (op_ptr->creates_frame) {
    DEBUG("[FRAME] Entering new frame for %d\n", op_ptr->op_code);
    push(fp);
    fp = sp;
  }

  int result = eval(op_ptr);

  if (op_ptr->creates_frame) {
    sp = fp;
    fp = pop(); // restore caller frame
  }

  push(result);
}

int eval(const struct op *op_ptr) {
  int op_code = op_ptr->op_code;

  int res;

  DEBUG("[EVAL] run_operator called with opcode = %d\n", op_code);

  switch (op_code) {
  case PUSH:
    return 0;
    break;

  default:
    fprintf(stderr, "Unknown op_code\n");
    return 0; // XXX FIX ME
    break;
  }
}

// // b = pop();    // right operand
// // a = pop();    // left operand
// // res = a OP b
// // and:
// // A OP= B → A = A OP B;
// int op_add(void) {
//     int res = 0;
//     while (sp > fp) res += pop();
//     printf("ADD → %d\n", res);
//     return res;
// }

// int op_sub(void) {
//     if (sp - fp < 1) {
//         fprintf(stderr, "SUB error: not enough values\n");
//         return 0;  // TODO: handle better
//     }

//     int res = pop();
//     while (sp > fp) {
//         res = pop() - res;
//     }
//     printf("SUB → %d\n", res);
//     return res;
// }

// int op_mul(void) {
//     int res = 1;
//     while (sp > fp) res *= pop();
//     printf("MUL → %d\n", res);
//     return res;
// }

// int op_div(void) {
//     if (sp - fp < 1) {
//         fprintf(stderr, "DIV error: not enough values\n");
//         return 0;  // TODO: handle better
//     }

//     int res = pop();
//     while (sp > fp) {
//         int divisor = res;
//         if (divisor == 0) {
//             fprintf(stderr, "Division by zero\n");
//             return 0;  // TODO: handle better
//         }
//         res = pop() / divisor;
//     }
//     printf("DIV → %d\n", res);
//     return res;
// }
