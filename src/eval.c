#include <assert.h>
#include <setjmp.h>
#include <stdio.h>

#include "core_def.h"
#include "debug.h"
#include "eval.h"

extern jmp_buf eval_error_jmp;

static void *raise(const char *msg) {
  fprintf(stderr, "Eval error: %s\n", msg);
  longjmp(eval_error_jmp, 1);
  return NULL;
}

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

Node *apply(Node *fn_node, Node *fn_args, Context *ctx) {
  DEBUG(DEBUG_LOCATION);
  // node must be a function either a closure or prim
  // fn_args must be a list of nodes
  // params must be a list of symbols
  // count of fn_args and params must be the same

  // if (is_closure_fn(node)) {
  //   Env *env = get_closure_env(node);
  //   Env *new_env = env_new(env); // TODO: error handling

  //   Node *params = get_closure_params(node)

  //   for (Node *param = get_closure_params(node);
  //        params; params =
  //     const char *symbol = params->as.literal.as.symbol;
  //     Node *arg = first(fn_args, ctx);

  //     env_set(new_env, symbol, arg);

  //   }

  //   Context new_ctx = *ctx;
  //   CTX_ENV(&new_ctx) = fn.as.closure.env;

  //   Node *body = fn.as.closure.body;
  //   return eval(body, &new_ctx);
  // }

  if (is_primitive_fn(fn_node)) {
    const PrimOp *prim_op = get_prim_op(fn_node);

    if (prim_op->unary_f_ptr)
      return prim_op->unary_f_ptr(first(fn_args, ctx), ctx);

    if (prim_op->binary_f_ptr) {
      Node *car = first(fn_args, ctx);
      Node *cdr = rest(fn_args, ctx);
      return prim_op->binary_f_ptr(car, cdr, ctx);
    }

    return raise("Unknown primitive in apply");
  }

  return raise("Unknown node type in apply");
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
  DEBUG(DEBUG_LOCATION);
  Node *node = cons_list(CTX_POOL(ctx), car, cdr);
  return node;
}

Node *first(Node *args, Context *ctx) {
  DEBUG(DEBUG_LOCATION);
  (void)ctx;
  if (!is_list(args)) {
    return raise("First only takes a list.");
  }
  return get_car(args);
}

Node *lookup(Node *node, Context *ctx) {
  DEBUG(DEBUG_LOCATION);
  if (!is_symbol(node)) {
    return raise("Lookup parameter to lookup must be a symbol.");
  }

  void *rval;

  if (env_lookup(CTX_ENV(ctx), get_symbol(node), &rval)) {
    return raise("Could not resolve symbol.");
  }

  return rval;
}

Node *quote(Node *args, Context *ctx) {
  /* only exists because it's a symbol. */
  (void)args;
  (void)ctx;
  return raise("Quote does not exist.");
  return NULL;
}

Node *rest(Node *args, Context *ctx) {
  DEBUG(DEBUG_LOCATION);
  (void)ctx;
  if (!is_list(args)) {
    return raise("Rest only takes a list.");
  }
  Node *cdr = get_cdr(args);
  return is_list(cdr) ? cdr : cons(cdr, NULL, ctx);
}

Node *set(Node *car, Node *cdr, Context *ctx) {
  DEBUG(DEBUG_LOCATION);

  if (!is_symbol(car)) {
    return raise("Set arameter to set must be a symbol.");
  }

  env_set(CTX_ENV(ctx), get_symbol(car), cdr); // TODO: error handling

  return cdr;
}

Node *eval(Node *expr, Context *ctx) {
  DEBUG(DEBUG_LOCATION);
  if (is_symbol(expr)) {
    return lookup(expr, ctx);
  }

  if (is_literal(expr) || is_function(expr)) {
    return expr;
  }

  if (is_list(expr)) {
    if (is_empty_list(expr)) {
      return expr;
    }

    Node *fn_node = eval(first(expr, ctx), ctx);

    if (PRIM_OP(QUOTE) != get_prim_op(fn_node)) {
      Node *fn_args = eval_list(rest(expr, ctx), ctx);
      return apply(fn_node, fn_args, ctx);
    } else
      return first(rest(expr, ctx), ctx);
  }

  return raise("Type unknown to eval.");
}

Node *eval_list(Node *list, Context *ctx) {
  DEBUG(DEBUG_LOCATION);
  if (is_empty_list(list))
    return empty_list(CTX_POOL(ctx));

  Node *car = eval(first(list, ctx), ctx);
  Node *cdr = eval_list(rest(list, ctx), ctx);

  return cons(car, cdr, ctx);
}

Node *eval_program(Node *program, Context *ctx) {
  Node *result = NULL;

  for (Node *expr = first(program, ctx); !is_empty_list(expr);
       expr = first(program, ctx)) {
    result = eval(expr, ctx);
    program = rest(program, ctx);
  }

  return result;
}
