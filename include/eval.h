#ifndef EVAL_H
#define EVAL_H

#include "core_def.h"

// #define EVAL_FMT "Eval error: %s\n"
// #define X "Params must have same length."
// #define X "Unknown primitive in apply."
// #define X "Unknown node type in apply."
// #define X "Closure: params and body must be a list."
// #define X "First only takes a list."
// #define X "len only takes a list."
// #define X "Lookup parameter to lookup must be a symbol."
// #define X "Could not resolve symbol."
// #define X "Pair: list1 and list2 must be a list."
// #define X "Quote does not exist."
// #define X "Rest only takes a list."
// #define X "Set parameter to set must be a symbol."
// #define X "Type unknown to eval."

Node *apply(Node *node, Node *args, Context *ctx);
Node *closure(Node *params, Node *body, Context *ctx);
Node *cons(Node *car, Node *cdr, Context *ctx);
Node *first(Node *node, Context *ctx);
Node *length(Node *list, Context *ctx);
Node *lookup(Node *node, Context *ctx);
Node *pair(Node *list1, Node *list2, Context *ctx);
Node *quote(Node *node, Context *void_ctx);
Node *rest(Node *node, Context *ctx);
Node *set(Node *car, Node *cdr, Context *ctx);
Node *eval(Node *expr, Context *ctx);
Node *eval_list(Node *list, Context *ctx);
Node *eval_program(Node *program, Context *ctx);

#endif
