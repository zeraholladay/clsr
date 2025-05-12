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
  fprintf(stderr, "*** eval error: %s: %s\n", err_code_msg, msg);
  longjmp(eval_error_jmp, 1);
}

// FIXME
static Node *get_true(Context *ctx) {
  return cons_primop(CTX_POOL(ctx), PRIMITIVE(T));
}

// FIXME
static Node *get_nil(Context *ctx) {
  return cons_primop(CTX_POOL(ctx), PRIMITIVE(NIL));
}

static Node *apply_closure(Node *fn_node, Node *arglist, Context *ctx) {
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
  Context new_ctx = *ctx;
  CTX_ENV(&new_ctx) = env_new(get_closure_env(fn_node));

  for (Node *pairs = pair(get_closure_params(fn_node), arglist, ctx);
       !is_empty_list(pairs); pairs = rest(pairs, ctx)) {

    Node *pair = first(pairs, ctx);
    set(first(pair, &new_ctx), first(rest(pair, &new_ctx), ctx), &new_ctx);
  }

  // eval
  return eval(get_closure_body(fn_node), &new_ctx); // TODO: eval_program()
}

static Node *apply_primitive(Node *fn_node, Node *arglist, Context *ctx) {
  const Primitive *prim_op = get_prim_op(fn_node);

  // TODO: validate arglist is correct for type
  switch (prim_op->type) {
  case PRIMITIVE_NULL:
    raise(ERR_INTERNAL, DEBUG_LOCATION);
    return NULL;
    break;

  case PRIMITIVE_UNARY_FN:
    return prim_op->unary_fn_ptr(first(arglist, ctx), ctx);
    break;

  case PRIMITIVE_BINARY_FN:
    return prim_op->binary_fn_ptr(first(arglist, ctx),
                                  first(rest(arglist, ctx), ctx), ctx);
    break;

  case PRIMITIVE_TERNARY_FN:
    return prim_op->ternary_fn_ptr(
        first(arglist, ctx), first(rest(arglist, ctx), ctx),
        first(rest(rest(arglist, ctx), ctx), ctx), ctx);
    break;

  default:
    raise(ERR_INTERNAL, DEBUG_LOCATION);
    return NULL;
    break;
  }
}

Node *apply(Node *fn_node, Node *arglist, Context *ctx) {
  if (!(is_closure_fn(fn_node) || is_primitive_fn(fn_node)) ||
      !is_list(arglist)) {
    raise(ERR_INVALID_ARG, __func__);
    return NULL;
  }

  if (is_closure_fn(fn_node)) {
    return apply_closure(fn_node, arglist, ctx);
  }

  if (is_primitive_fn(fn_node)) {
    return apply_primitive(fn_node, arglist, ctx);
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
  Node *node = cons_list(CTX_POOL(ctx), car, cdr);
  return node;
}

Node *eq(Node *node1, Node *node2, Context *ctx) {
  return (type(node1)->eq_fn(node1, node2)) ? get_true(ctx) : get_nil(ctx);
}

Node *first(Node *list, Context *ctx) {
  (void)ctx;
  if (!is_list(list)) {
    raise(ERR_INVALID_ARG, __func__);
    return NULL;
  }
  return get_car(list);
}

Node *_if(Node *_bool, Node *then, Node *_else, Context *ctx) {
  Node *branch =
      type(get_true(ctx))->eq_fn(_bool, get_true(ctx)) ? then : _else;
  return eval(branch, ctx);
}

static size_t _length(Node *list) {
  size_t i = 0;
  for (Node *cdr = get_cdr(list); cdr; cdr = get_cdr(cdr))
    ++i;
  return i;
}

Node *length(Node *list, Context *ctx) {
  if (!is_list(list)) {
    raise(ERR_INVALID_ARG, __func__);
    return NULL;
  }
  return cons_integer(CTX_POOL(ctx), _length(list));
}

Node *list(Node *car, Node *cdr, Context *ctx) {
  Node *empty = empty_list(ctx);
  return cons(car, cons(cdr, empty, ctx), ctx);
}

Node *lookup(Node *node, Context *ctx) {
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

Node *print(Node *node, Context *ctx) {
  (void)ctx;
  char *str = type(node)->str_fn(node);
  printf("%s\n", str);
  free(str);
  return get_true(ctx);
}

Node *repr(Node *node, Context *ctx) {
  (void)node;
  (void)ctx;
  return NULL;
}

Node *rest(Node *list, Context *ctx) {
  (void)ctx;
  if (!is_list(list)) {
    raise(ERR_INVALID_ARG, __func__);
    return NULL;
  }
  Node *cdr = get_cdr(list);
  return is_list(cdr) ? cdr : cons(cdr, NULL, ctx);
}

Node *set(Node *car, Node *cdr, Context *ctx) {
  if (!is_symbol(car)) {
    raise(ERR_INVALID_ARG, __func__);
    return NULL;
  }
  env_set(CTX_ENV(ctx), get_symbol(car), cdr); // TODO: error handling
  return cdr;
}

Node *_str(Node *node, Context *ctx) {
  return cons_string(CTX_POOL(ctx), type(node)->str_fn(node));
}

Node *eval(Node *expr, Context *ctx) {
  if (is_symbol(expr)) {
    return lookup(expr, ctx);
  }

  if (is_literal(expr) || is_function(expr) || is_string(expr)) {
    return expr;
  }

  if (is_list(expr)) {
    if (is_empty_list(expr)) {
      return expr;
    }

    Node *fn_node = eval(first(expr, ctx), ctx);

    if (PRIMITIVE(QUOTE) == get_prim_op(fn_node)) {
      return first(rest(expr, ctx), ctx);
    }

    Node *arglist = eval_list(rest(expr, ctx), ctx);

    return (PRIMITIVE(APPLY) == get_prim_op(fn_node))
               ? apply(first(arglist, ctx), rest(arglist, ctx), ctx)
               : apply(fn_node, arglist, ctx);
  }

  raise(ERR_INTERNAL, DEBUG_LOCATION);
  return NULL;
}

Node *eval_list(Node *list, Context *ctx) {
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
