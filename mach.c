#include "mach.h"

#ifndef STACK_SIZE
#define STACK_SIZE 1024
#endif
int stack[STACK_SIZE];
int sp = 0;  // stack pointer
int fp = 0;  // frame pointer

void push(int val) {
    if (sp >= STACK_SIZE) {
        fprintf(stderr, "Stack overflow");
        return;
    }
    stack[sp++] = val;
}

int pop() {
    if (sp <= 0) {
        fprintf(stderr, "Stack underflow");
        return 0;
    }
    return stack[--sp];
}

void enter_frame() {
    push(fp);
    fp = sp;
}

void run_operator(const struct op *op_ptr) {
    // b = pop();    // right operand
    // a = pop();    // left operand
    // res = a OP b
    // and:
    // A OP= B → A = A OP B;

    int op_code = op_ptr->op_code;

    int res;

    DEBUG("run_operator called with opcode = %d\n", op_code);

    switch (op_code) {
        case ADD:
            res = 0;
            while (sp > fp) res += pop();  // res = res + pop() 
            printf("ADD → %d\n", res);
            fp = pop();
            push(res);
            break;

        case SUB:
            if (sp - fp < 1) {
                fprintf(stderr, "SUB error: not enough values\n");
                return;
            }
            
            res = pop();  // rightmost argument
            
            while (sp > fp) {
                res = pop() - res;  // left-associative
            }
            
            fp = pop();  // restore previous frame pointer
            push(res);
            printf("SUB → %d\n", res);
            break;        

        case MUL:
            res = 1;
            while (sp > fp) res *= pop(); // res = res * pop() 
            printf("MUL → %d\n", res);
            fp = pop();
            push(res);
            break;

        case DIV:
            if (sp - fp < 1) {
                fprintf(stderr, "DIV error: not enough values\n");
                return;
            }
            
            int res = pop();  // first value popped is the last argument (right-most)
            
            while (sp > fp) {
                if (res == 0) {
                    fprintf(stderr, "Division by zero\n");
                    return;
                }
                res = pop() / res;  // do pop() / res, so evaluation is left-to-right
            }
            
            fp = pop();
            push(res);
            printf("DIV → %d\n", res);
            break;
        

        default:
            fprintf(stderr, "Unknown op_code");
            break;
    }
}
