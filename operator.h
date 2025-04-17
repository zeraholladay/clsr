#ifndef OPERATOR_H
#define OPERATOR_H

struct op {
  int op_code;
  int creates_frame;
};

const struct op *lookup_op(register const char *str, register unsigned int len);
#endif