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

static size_t _length(Node *list);

static Node *_apply_closure(Node *fn_node, Node *arglist, Context *ctx) {
  DEBUG(DEBUG_LOCATION);

  Env *new_env = env_new(get_closure_env(fn_node)); // TODO: error handling
  Node *params = get_closure_params(fn_node);

  if (_length(params) != _length(arglist))
    raise("Params must have same length.");

  for (Node *pairs = pair(params, arglist, ctx); !is_empty_list(pairs);
       pairs = rest(pairs, ctx)) {
    Node *pair = first(pairs, ctx);
    // TODO: verify if symbol
    const char *symbol = get_symbol(first(pair, ctx));
    env_set(new_env, symbol,
            first(rest(pair, ctx), ctx)); // TODO: error handling
  }

  Context new_ctx = *ctx;
  CTX_ENV(&new_ctx) = new_env;

  return eval(get_closure_body(fn_node), &new_ctx);
}

Node *apply(Node *fn_node, Node *arglist, Context *ctx) {
  DEBUG(DEBUG_LOCATION);

  if (is_closure_fn(fn_node)) {
    return _apply_closure(fn_node, arglist, ctx);
  }

  if (is_primitive_fn(fn_node)) {
    const PrimOp *prim_op = get_prim_op(fn_node);

    if (prim_op->unary_f_ptr) {
      return prim_op->unary_f_ptr(get_car(arglist), ctx);
    }

    if (prim_op->binary_f_ptr) {
      return prim_op->binary_f_ptr(get_car(arglist), get_car(get_cdr(arglist)),
                                   ctx);
    }

    return raise("Unknown primitive in apply.");
  }

  return raise("Unknown node type in apply.");
}

Node *closure(Node *params, Node *body, Context *ctx) {
  if (!is_list(params)) {
    return raise("Closure: params and body must be a list.");
  }
  return cons_closure(CTX_POOL(ctx), params, body, CTX_ENV(ctx));
}

Node *cons(Node *car, Node *cdr, Context *ctx) {
  DEBUG(DEBUG_LOCATION);
  Node *node = cons_list(CTX_POOL(ctx), car, cdr);
  return node;
}

Node *first(Node *list, Context *ctx) {
  DEBUG(DEBUG_LOCATION);
  (void)ctx;
  if (!is_list(list)) {
    return raise("First only takes a list.");
  }
  return get_car(list) ? get_car(list) : cons(NULL, NULL, ctx);
}

static size_t _length(Node *list) {
  size_t i = 0;
  for (Node *cdr = get_cdr(list); cdr; cdr = get_cdr(cdr))
    ++i;
  return i;
}

Node *length(Node *list, Context *ctx) {
  DEBUG(DEBUG_LOCATION);
  if (!is_list(list)) {
    return raise("len only takes a list.");
  }
  return cons_integer(CTX_POOL(ctx), _length(list));
}

Node *list(Node *car, Node *cdr, Context *ctx) {
  DEBUG(DEBUG_LOCATION);
  Node *empty = empty_list(ctx);
  return cons(car, cons(cdr, empty, ctx), ctx);
}

Node *lookup(Node *node, Context *ctx) {
  DEBUG(DEBUG_LOCATION);
  if (!is_symbol(node)) {
    return raise("Lookup parameter to lookup must be a symbol.");
  }

  rb_node *n = env_lookup(CTX_ENV(ctx), get_symbol(node));

  if (!n) {
    return raise("Could not resolve symbol.");
  }

  return RB_VAL(n);
}

Node *pair(Node *list1, Node *list2, Context *ctx) {
  DEBUG(DEBUG_LOCATION);
  if (!is_list(list1) || !is_list(list2)) {
    return raise("Pair: list1 and list2 must be a list.");
  }

  if (is_empty_list(list1) || is_empty_list(list2))
    return cons(NULL, NULL, ctx);

  Node *first_pair = list(first(list1, ctx), first(list2, ctx), ctx);
  Node *rest_pairs = pair(rest(list1, ctx), rest(list2, ctx), ctx);

  return cons(first_pair, rest_pairs, ctx);
}

Node *quote(Node *list, Context *ctx) {
  /* only exists because it's a symbol. */
  (void)list;
  (void)ctx;
  return raise("Quote does not exist.");
  return NULL;
}

Node *rest(Node *list, Context *ctx) {
  DEBUG(DEBUG_LOCATION);
  (void)ctx;
  if (!is_list(list)) {
    return raise("Rest only takes a list.");
  }
  Node *cdr = get_cdr(list);
  return is_list(cdr) ? cdr : cons(cdr, NULL, ctx);
}

Node *set(Node *car, Node *cdr, Context *ctx) {
  DEBUG(DEBUG_LOCATION);

  if (!is_symbol(car)) {
    return raise("Set parameter to set must be a symbol.");
  }

  cdr = (is_list(cdr)) ? first(cdr, ctx) : cdr;

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

    if (PRIM_OP(QUOTE) == get_prim_op(fn_node)) {
      return first(rest(expr, ctx), ctx);
    }

    Node *arglist = eval_list(rest(expr, ctx), ctx);
    return apply(fn_node, arglist, ctx);
  }

  return raise("Type unknown to eval.");
}

Node *eval_list(Node *list, Context *ctx) {
  DEBUG(DEBUG_LOCATION);
  if (is_empty_list(list))
    return empty_list(ctx);

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
