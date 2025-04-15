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
        fprintf(stderr, "Stack underflow\n");
        return 0;
    }
    return stack[--sp];
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
        fp = pop();  // restore caller frame
    }

    push(result);
}

int eval(const struct op *op_ptr) {
    // b = pop();    // right operand
    // a = pop();    // left operand
    // res = a OP b
    // and:
    // A OP= B → A = A OP B;

    int op_code = op_ptr->op_code;

    int res;

    DEBUG("[EVAL] run_operator called with opcode = %d\n", op_code);

    switch (op_code) {
        case PUSH:
            return 0;
            break;

        case ADD:
            res = 0;
            while (sp > fp) res += pop();  // res = res + pop() 
            printf("ADD → %d\n", res);
            return res;
            break;

        case SUB:
            if (sp - fp < 1) {
                fprintf(stderr, "SUB error: not enough values\n");
                return 0; //XXX Fix me
            }
            
            res = pop();  // rightmost argument
            
            while (sp > fp) {
                res = pop() - res;  // left-associative
            }
            printf("SUB → %d\n", res);
            return res;
            break;        

        case MUL:
            res = 1;
            while (sp > fp) res *= pop(); // res = res * pop() 
            printf("MUL → %d\n", res);
            return res;
            break;

        case DIV:
            if (sp - fp < 1) {
                fprintf(stderr, "DIV error: not enough values\n");
                return 0;  //XXX FIX ME
            }
            
            int res = pop();  // first value popped is the last argument (right-most)
            
            while (sp > fp) {
                if (res == 0) {
                    fprintf(stderr, "Division by zero\n");
                    return 0;  //XXX FIX ME
                }
                res = pop() / res;  // do pop() / res, so evaluation is left-to-right
            }
            
            printf("DIV → %d\n", res);
            return res;
            break;
        

        default:
            fprintf(stderr, "Unknown op_code\n");
            return 0;  //XXX FIX ME
            break;
    }
}
