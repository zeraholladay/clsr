#ifndef EVAL_H
#define EVAL_H

#include "core_def.h"

typedef enum {
  ERR_INTERNAL,           // Internal error (bug)
  ERR_SYMBOL_NOT_FOUND,   // Symbol undefined
  ERR_INVALID_ARG,        // Invalid type or value for argument
  ERR_MISSING_ARG,        // Missing required argument
  ERR_ARG_TYPE_MISMATCH,  // Argument has wrong type
  ERR_ARG_OUT_OF_RANGE,   // Value out of range
  ERR_UNEXPECTED_ARG,     // Extra or unexpected argument
  ERR_INVALID_ARG_LENGTH, // Length of argument is invalid
  ERR_NULL_ARG,           // Null argument where not allowed
  ERR_ARG_NOT_ITERABLE,   // Argument expected to be iterable
} ErrorCode;

static const char *error_messages[] = {
    [ERR_INTERNAL] = "Internal error occurred.",
    [ERR_SYMBOL_NOT_FOUND] = "Could not resolve symbol.",
    [ERR_INVALID_ARG] = "Invalid argument type or value.",
    [ERR_MISSING_ARG] = "Missing required argument.",
    [ERR_ARG_TYPE_MISMATCH] = "Argument type mismatch.",
    [ERR_ARG_OUT_OF_RANGE] = "Argument value out of range.",
    [ERR_UNEXPECTED_ARG] = "Unexpected argument provided.",
    [ERR_INVALID_ARG_LENGTH] = "Invalid argument length.",
    [ERR_NULL_ARG] = "Argument cannot be null.",
    [ERR_ARG_NOT_ITERABLE] = "Argument is not iterable when expected.",
};

Node *apply(Node *node, Node *args, Context *ctx);
Node *closure(Node *params, Node *body, Context *ctx);
Node *cons(Node *car, Node *cdr, Context *ctx);
Node *eq(Node *node1, Node *node2, Context *ctx);
Node *first(Node *node, Context *ctx);
Node *length(Node *list, Context *ctx);
Node *lookup(Node *node, Context *ctx);
Node *pair(Node *list1, Node *list2, Context *ctx);
Node *print(Node *node, Context *ctx);
Node *repr(Node *node, Context *ctx);
Node *rest(Node *node, Context *ctx);
Node *set(Node *car, Node *cdr, Context *ctx);
Node *_str(Node *node, Context *ctx);
Node *eval(Node *expr, Context *ctx);
Node *eval_list(Node *list, Context *ctx);
Node *eval_program(Node *program, Context *ctx);

#endif
