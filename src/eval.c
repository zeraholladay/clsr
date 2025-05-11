#include <assert.h>
#include <setjmp.h>
#include <stdio.h>

#include "core_def.h"
#include "debug.h"
#include "eval.h"

extern jmp_buf eval_error_jmp;

static size_t _length(Node *list);

static void raise(ErrorCode err_code, const char *msg) {
  const char *err_code_msg = error_messages[err_code];
  fprintf(stderr, "*** eval error: %s\n  %s\n", err_code_msg, msg);
  longjmp(eval_error_jmp, 1);
}

static Node *apply_closure(Node *fn_node, Node *arglist, Context *ctx) {
  DEBUG(DEBUG_LOCATION);
  // validate
  size_t params_len = _length(get_closure_params(fn_node));
  size_t arglist_len = _length(arglist);

  if (params_len > arglist_len) {
    raise(ERR_MISSING_ARG, __func__); // FIXME
    return NULL;
  }

  if (params_len < arglist_len) {
    raise(ERR_UNEXPECTED_ARG, __func__); // FIXME
    return NULL;
  }

  // setup context
  Env *new_env = env_new(get_closure_env(fn_node)); // TODO: error handling

  for (Node *pairs = pair(get_closure_params(fn_node), arglist, ctx);
       !is_empty_list(pairs); pairs = rest(pairs, ctx)) {

    Node *pair = first(pairs, ctx);

    if (!is_symbol(first(pair, ctx))) {
      raise(ERR_INVALID_ARG, __func__); // FIXME
      return NULL;
    }

    const char *symbol = get_symbol(first(pair, ctx));
    env_set(new_env, symbol,
            first(rest(pair, ctx), ctx)); // TODO: error handling
  }

  Context new_ctx = *ctx;
  CTX_ENV(&new_ctx) = new_env;

  // eval
  return eval(get_closure_body(fn_node), &new_ctx);
}

static Node *apply_prim_op(Node *fn_node, Node *arglist, Context *ctx) {
  const PrimOp *prim_op = get_prim_op(fn_node);

  switch (prim_op->kind) {
  case PRIM_OP_NULL:
    raise(ERR_INTERNAL, DEBUG_LOCATION);
    return NULL;
    break;

  case PRIM_OP_UNARY_FN:
    return prim_op->unary_fn_ptr(get_car(arglist), ctx);
    break;

  case PRIM_OP_BINARY_FN:
    return prim_op->binary_fn_ptr(get_car(arglist), get_car(get_cdr(arglist)),
                                  ctx);
    break;

  default:
    raise(ERR_INTERNAL, DEBUG_LOCATION);
    return NULL;
    break;
  }
}

Node *apply(Node *fn_node, Node *arglist, Context *ctx) {
  DEBUG(DEBUG_LOCATION);

  if (!(is_closure_fn(fn_node) || is_primitive_fn(fn_node)) ||
      !is_list(arglist)) {
    raise(ERR_INVALID_ARG, __func__);
    return NULL;
  }

  if (is_closure_fn(fn_node)) {
    return apply_closure(fn_node, arglist, ctx);
  }

  if (is_primitive_fn(fn_node)) {
    return apply_prim_op(fn_node, arglist, ctx);
  }

  raise(ERR_INTERNAL, DEBUG_LOCATION);
  return NULL;
}

Node *closure(Node *params, Node *body, Context *ctx) {
  if (!is_list(params)) {
    raise(ERR_INVALID_ARG, __func__);
    return NULL;
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
    raise(ERR_INVALID_ARG, __func__);
    return NULL;
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
    raise(ERR_INVALID_ARG, __func__);
    return NULL;
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
    raise(ERR_INVALID_ARG, __func__);
    return NULL;
  }

  rb_node *n = env_lookup(CTX_ENV(ctx), get_symbol(node));

  if (!n) {
    raise(ERR_SYMBOL_NOT_FOUND, get_symbol(node));
    return NULL;
  }

  return RB_VAL(n);
}

Node *pair(Node *list1, Node *list2, Context *ctx) {
  DEBUG(DEBUG_LOCATION);
  if (!is_list(list1) || !is_list(list2)) {
    raise(ERR_INVALID_ARG, __func__);
    return NULL;
  }

  if (is_empty_list(list1) || is_empty_list(list2))
    return cons(NULL, NULL, ctx);

  Node *first_pair = list(first(list1, ctx), first(list2, ctx), ctx);
  Node *rest_pairs = pair(rest(list1, ctx), rest(list2, ctx), ctx);

  return cons(first_pair, rest_pairs, ctx);
}

Node *repr(Node *node, Context *ctx) {
  DEBUG(DEBUG_LOCATION);
  (void)node;
  (void)ctx;
  return NULL;
}

Node *rest(Node *list, Context *ctx) {
  DEBUG(DEBUG_LOCATION);
  (void)ctx;
  if (!is_list(list)) {
    raise(ERR_INVALID_ARG, __func__);
    return NULL;
  }
  Node *cdr = get_cdr(list);
  return is_list(cdr) ? cdr : cons(cdr, NULL, ctx);
}

Node *set(Node *car, Node *cdr, Context *ctx) {
  DEBUG(DEBUG_LOCATION);

  if (!is_symbol(car)) {
    raise(ERR_INVALID_ARG, __func__);
    return NULL;
  }

  cdr = (is_list(cdr)) ? first(cdr, ctx) : cdr;

  env_set(CTX_ENV(ctx), get_symbol(car), cdr); // TODO: error handling

  return cdr;
}

Node *_str(Node *node, Context *ctx) {
  DEBUG(DEBUG_LOCATION);
  (void)node;
  (void)ctx;
  return NULL;
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

  raise(ERR_INTERNAL, DEBUG_LOCATION);
  return NULL;
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
