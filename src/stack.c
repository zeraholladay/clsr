#include "stack.h"

Stack STACK = {0};  // temporary

// #ifndef STACK_SIZE
// #define STACK_SIZE 1024
// #endif

// void push(int val) {
//   if (stack.sp >= STACK_SIZE) {
//     ERRMSG("Stack overflow");
//     return;
//   }
//   stack.stack[stack.sp++] = val;
// }

// int pop() {
//   if (stack.sp <= 0) {
//     ERRMSG("Stack underflow\n");
//     return 0;
//   }
//   return stack.stack[--stack.sp];
// }