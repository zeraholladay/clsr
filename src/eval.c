#include <assert.h>

#include "core_def.h"
#include "debug.h"
#include "eval.h"

static inline int is_literal(const Node *node) {
  return node && node->kind == KIND_LITERAL;
}

static inline int is_list(const Node *node) {
  return node && node->kind == KIND_LIST;
}

static inline int is_function(const Node *node) {
  return node && node->kind == KIND_FUNCTION;
}

// Literal kind checks
static inline int is_integer(const Node *node) {
  return is_literal(node) && node->as.literal.kind == LITERAL_INTEGER;
}

static inline int is_keyword(const Node *node) {
  return is_literal(node) && node->as.literal.kind == LITERAL_KEYWORD;
}

static inline int is_symbol(const Node *node) {
  return is_literal(node) && node->as.literal.kind == LITERAL_SYMBOL;
}

// Function kind checks
static inline int is_primitive_fn(const Node *node) {
  return is_function(node) && node->as.function.kind == FN_PRIMITIVE;
}

static inline int is_closure_fn(const Node *node) {
  return is_function(node) && node->as.function.kind == FN_CLOSURE;
}

static inline int is_empty_list(const Node *node) {
  return !node || (is_list(node) && !node->as.list.car && !node->as.list.cdr);
}
static inline Node *empty_list(Pool *p) { return cons_list(p, NULL, NULL); }

// Literal accessors
static inline Literal *get_literal(Node *node) {
  return is_literal(node) ? &node->as.literal : NULL;
}

static inline int get_integer(Node *node) {
  return is_integer(node) ? node->as.literal.as.integer : 0;
}

static inline const char *get_symbol(Node *node) {
  return (is_symbol(node) || is_keyword(node)) ? node->as.literal.as.symbol
                                               : NULL;
}

// List accessors
static inline List *get_list(Node *node) {
  return is_list(node) ? &node->as.list : NULL;
}

static inline Node *get_car(Node *node) {
  return is_list(node) ? node->as.list.car : NULL;
}

static inline Node *get_cdr(Node *node) {
  return is_list(node) ? node->as.list.cdr : NULL;
}

// Function accessors
static inline Function *get_function(Node *node) {
  return is_function(node) ? &node->as.function : NULL;
}

static inline PrimOp *get_prim_op(Node *node) {
  return is_primitive_fn(node) ? node->as.function.as.primitive.prim_op : NULL;
}

static inline Node *get_closure_params(Node *node) {
  return is_closure_fn(node) ? node->as.function.as.closure.params : NULL;
}

static inline Node *get_closure_body(Node *node) {
  return is_closure_fn(node) ? node->as.function.as.closure.body : NULL;
}

static inline Env *get_closure_env(Node *node) {
  return is_closure_fn(node) ? node->as.function.as.closure.env : NULL;
}

Node *apply(Node *node, Node *args, Context *ctx) {
  // node must be a function either a closure or prim
  // args must be a list of nodes
  // params must be a list of symbols
  // count of args and params must be the same

  // if (is_closure_fn(node)) {
  //   Env *env = get_closure_env(node);
  //   Env *new_env = env_new(env); // TODO: error handling

  //   Node *params = get_closure_params(node)

  //   for (Node *param = get_closure_params(node);
  //        params; params =
  //     const char *symbol = params->as.literal.as.symbol;
  //     Node *arg = first(args, ctx);

  //     env_set(new_env, symbol, arg);

  //   }

  //   Context new_ctx = *ctx;
  //   CTX_ENV(&new_ctx) = fn.as.closure.env;

  //   Node *body = fn.as.closure.body;
  //   return eval(body, &new_ctx);
  // }

  node_fprintf(stderr, node), fprintf(stderr, "< node\n");
  node_fprintf(stderr, args), fprintf(stderr, "< args\n");

  if (is_primitive_fn(node)) {
    const PrimOp *prim_op = get_prim_op(node);

    if (prim_op->unary_f_ptr) {
      Node *cdr = first(args, ctx);
      return prim_op->unary_f_ptr(cdr, ctx);
    } else if (prim_op->binary_f_ptr) {
      Node *car = first(args, ctx);
      Node *cdr = rest(args, ctx);
      return prim_op->binary_f_ptr(car, cdr, ctx);
    } else {
      return NULL;
    }
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

Node *cons(Node *car, Node *cdr, Context *ctx) {
  Node *node = cons_list(CTX_POOL(ctx), car, cdr);
  return node;
}

Node *first(Node *node, Context *ctx) {
  (void)ctx;
  return get_car(node);
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
    return NULL; // TODO: error handling
  }

  void *rval;

  if (env_lookup(CTX_ENV(ctx), get_symbol(node), &rval)) {
    return NULL;
  }

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

Node *quote(Node *node, Context *ctx) {
  (void)ctx;
  return is_list(node) ? node : NULL;
}

Node *rest(Node *node, Context *ctx) {
  (void)ctx;
  return get_cdr(node);
}

// Node *return_(Node *void_Node, Context *ctx) {
//   (void)void_Node;

//   Node *Node_rval = CTX_POP(ctx);
//   EXIT_FRAME(CTX_STACK(ctx));
//   CTX_PUSH(ctx, Node_rval);

//   return Node_true;
// }

Node *set(Node *node, Context *ctx) {
  Node *car = first(node, ctx);
  Node *cdr = rest(node, ctx);

  node_fprintf(stdout, node), fprintf(stdout, " <= set\n");

  if (!is_symbol(car)) {
    assert(0 && "Node is not a literal symbol.");
    return NULL; // TODO: error handling
  }

  env_set(CTX_ENV(ctx), get_symbol(car), cdr); // TODO: error handling

  return cdr;
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

    Node *car = first(expr, ctx);
    Node *cdr = rest(expr, ctx);

    Node *fn = eval(car, ctx);
    Node *evaluated_args = eval_list(cdr, ctx);

    return apply(fn, evaluated_args, ctx);
  }

  assert(0 && "unknown to eval");
  return NULL;
}

Node *eval_list(Node *list, Context *ctx) {
  if (!list || is_empty_list(list))
    return empty_list(CTX_POOL(ctx));

  Node *car = eval(first(list, ctx), ctx);
  Node *cdr = eval_list(rest(list, ctx), ctx);

  return cons(car, cdr, ctx);
}

Node *eval_program(Node *program, Context *ctx) {
  Node *result = NULL;

  node_fprintf(stderr, program), fprintf(stderr, "< program\n");

  for (Node *expr = first(program, ctx); expr; expr = first(program, ctx)) {
    node_fprintf(stderr, expr), fprintf(stderr, "< expr\n");
    // result = eval(expr, ctx);
    program = rest(program, ctx);
  }

  return result;
}
