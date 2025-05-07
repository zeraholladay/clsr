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

// arglist = ((arg1) (arg2) .. (argN))
Node *apply(Node *fn_node, Node *arglist, Context *ctx) {
  DEBUG(DEBUG_LOCATION);

  if (is_closure_fn(fn_node)) {
    Env *env = get_closure_env(fn_node);
    Env *new_env = env_new(env); // TODO: error handling

    Node *params = get_closure_params(fn_node);
    arglist = first(arglist, ctx);

    while (!is_empty_list(arglist) && !is_empty_list(params)) {
      const char *symbol = get_symbol(first(params, ctx));
      Node *value = first(arglist, ctx);

      env_set(new_env, symbol, value); // TODO: error handling

      arglist = rest(arglist, ctx);
      params = rest(params, ctx);
    }
    // XXX check lists are both empty

    Context new_ctx = *ctx;
    CTX_ENV(&new_ctx) = new_env;

    Node *body = first(get_closure_body(fn_node), ctx);

    return eval(body, &new_ctx);
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
  if (!is_list(params) || !is_list(body)) {
    return raise("Closure: params and body must be a list.");
  }
  return cons_closure(CTX_POOL(ctx), params, body);
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
