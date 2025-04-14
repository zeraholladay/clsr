#ifndef OP_H
#define OP_H

struct op {
    int op_code;
};

const struct op * lookup_op(register const char *str, register unsigned int len);
#endif