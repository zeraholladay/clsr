#include <assert.h>

#include "common.h"
#include "core_def.h"
#include "core_list.h"
#include "eval.h"

extern Node *const const_false;
extern Node *const const_true;

static Node *apply(Node *node, Node *args, Context *ctx) {
  // node must be a function either a closure or prim
  // args must be a list of nodes
  // params must be a list of symbols
  // count of args and params must be the same

  Function fn = node->as.function;
  List *fn_args = args->as.list;

  if (is_closure_fn(node)) {
    List *params = fn.as.closure.params->as.list;
    Node *body = fn.as.closure.body;
    Env *env = fn.as.closure.env;

    Env *new_env = env_new(env); // TODO: error handling

    for (unsigned int i = 0; i < params->count; ++i) {
      const char *symbol = params->items[i]->as.literal.as.symbol;
      env_set(new_env, symbol, fn_args->items[i]);
    }

    Context new_ctx = *ctx;
    CTX_ENV(&new_ctx) = env;

    return eval(body, &new_ctx);
  }

  if (is_primitive_fn(node)) {
    const PrimFunc fn_ptr = fn.as.primitive.fn_ptr;
    return fn_ptr(args, ctx);
  }

  return NULL;
}

// Node *closure(Node *Node, Context *ctx) {
//   if (!Node_IS_CLOSURE(Node)) {
//     assert(0 && "Node is not a closure.");
//     return false; // TODO: error handling
//   }

//   Closure *closure = Node_AS_PTR(Node, closure);
//   closure->env = CTX_ENV(ctx);
//   CTX_PUSH(ctx, Node);
//   return Node_true;
// }

Node *cons(Node *first, Node *rest, Context *ctx) {
  Node *node = list_cons(CTX_POOL(ctx), NULL, NULL);

  

  return node;
}

// Node *eq(Node *Node, Context *ctx) {
//   Node *Node1 = CTX_POP(ctx);
//   Node *Node2 = CTX_POP(ctx);

//   // Literal ints are special
//   if (Node_IS_LITERAL_INT(Node1) && Node_IS_LITERAL_INT(Node2)) {
//     Node *result = Node_AS(Node1, literal).integer == Node_AS(Node2,
//     literal).integer
//                       ? Node_true
//                       : false;
//     CTX_PUSH(ctx, result);
//     return Node_true;
//   }

//   // Symbols, keyword literals, lists, calls, closures, ifs, etc.
//   // if we get lists, a comparison would be cool.
//   CTX_PUSH(ctx, Node2);
//   CTX_PUSH(ctx, Node1);
//   return is(Node, ctx);
// }

// Node *if_(Node *Node, Context *ctx) {
//   if (!Node_IS_IF(Node)) {
//     assert(0 && "Node is not an if.");
//     return false; // TODO: error handling
//   }

//   Node *cond = CTX_POP(ctx);

//   if (!cond) {
//     return false; // TODO: error handling
//   }

//   NodeIf Node_if = Node_AS(Node, if_);

//   Node *branch = (cond == Node_true) ? Node_if.then : Node_if.else_;

//   return eval(branch, ctx);
// }

// Node *is(Node *void_Node, Context *ctx) {
//   (void)void_Node;

//   Node *Node1 = CTX_POP(ctx);
//   Node *Node2 = CTX_POP(ctx);

//   Node *result = NULL;

//   // symbols are globally unique
//   if (Node_IS_LITERAL_SYM(Node1) && Node_IS_LITERAL_SYM(Node1)) {
//     const char *sym1 = Node_AS(Node1, literal).symbol;
//     const char *sym2 = Node_AS(Node2, literal).symbol;
//     result = (uintptr_t)sym1 == (uintptr_t)sym2 ? Node_true : false;
//   } else
//     result = (uintptr_t)Node1 == (uintptr_t)Node2 ? Node_true : false;

//   CTX_PUSH(ctx, result);

//   return Node_true;
// }

Node *lookup(Node *node, Context *ctx) {
  if (!is_symbol(node)) {
    return const_false; // TODO: error handling
  }

  void *rval;

  if (env_lookup(CTX_ENV(ctx), node->as.literal.as.symbol, &rval))
    return const_false;

  return rval;
}

// Node *push(Node *Node, Context *ctx) {
//   NodeCall Node_call = Node_AS(Node, call);
//   jList *list = Node_AS(Node_call.args, list);

//   for (unsigned int i = list->count; i > 0; --i) {
//     assert(Node_ISKIND(list->nodes[i - 1], Node_Literal));
//     CTX_PUSH(ctx, list->nodes[i - 1]);
//   }

//   return Node_true;
// }

Node *quote(Node *args, Context *ctx) {
  if (!is_list(args)) {
    return NULL;
  }
  return list_rest(CTX_POOL(ctx), args);
}

// Node *return_(Node *void_Node, Context *ctx) {
//   (void)void_Node;

//   Node *Node_rval = CTX_POP(ctx);
//   EXIT_FRAME(CTX_STACK(ctx));
//   CTX_PUSH(ctx, Node_rval);

//   return Node_true;
// }

Node *set(Node *args, Context *ctx) {
  Node *first = list_first(args);
  Node *rest = list_rest(CTX_POOL(ctx), args);

  if (!is_symbol(first)) {
    assert(0 && "Node is not a literal symbol.");
    return const_false; // TODO: error handling
  }

  env_set(CTX_ENV(ctx), first->as.literal.as.symbol, rest); // TODO: error handling

  return rest;
}

Node *eval(Node *expr, Context *ctx) {
  if (is_symbol(expr)) {
    return lookup(expr, ctx);
  }

  if (is_literal(expr)) { // othere literals
    return expr;
  }

  if (is_function(expr)) {
    return expr;
  }

  if (is_list(expr)) {
    if (is_empty_list(expr)) {
      return expr; // NIL or '()
    }

    Node *op = list_first(expr);
    Node *args = list_rest(CTX_POOL(ctx), expr);

    Node *fn = eval(op, ctx);
    Node *evaluated_args = eval_list(args, ctx);
    return apply(fn, evaluated_args, ctx);
  }

  assert(0 && "unknown to eval");
  return NULL;
}

Node *eval_list(Node *list, Context *ctx) {
  if (is_empty_list(list))
    return empty_list(CTX_POOL(ctx));

  Node *first = eval(list_first(list), ctx);
  Node *rest = eval_list(list_rest(CTX_POOL(ctx), list), ctx);

  return cons(first, rest, ctx);
}

Node *eval_program(Node *program, Context *ctx) {
  List *expressions = program->as.list;
  Node *result = const_false;

  for (unsigned int i = 0; i < expressions->count; ++i) {
    result = eval(expressions->items[i], ctx);
  }

  return result;
}
