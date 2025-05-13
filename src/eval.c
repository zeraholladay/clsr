#include <assert.h>
#include <setjmp.h>
#include <stdio.h>

#include "core_def.h"
#include "debug.h"
#include "eval.h"

extern jmp_buf eval_error_jmp;

static size_t _length(Node *list);
static Node *pair(Node *list1, Node *list2, Context *ctx);
static Node *set(Node *car, Node *cdr, Context *ctx);

static void raise(ErrorCode err_code, const char *msg) {
  const char *err_code_msg = error_messages[err_code];
  fprintf(stderr, "*** eval error: %s: %s\n", err_code_msg, msg);
  longjmp(eval_error_jmp, 1);
}

static Node *nth(Node *list, size_t n) {
  for (size_t i = 0; i < n; i++) {
    if (!list)
      return NULL;
    list = get_car(list);
  }
  return get_cdr(list);
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
       !is_empty_list(pairs); pairs = REST(pairs)) {

    Node *pair = FIRST(pairs);
    set(FIRST(pair), FIRST(REST(pair)), &new_ctx);
  }

  // eval
  return eval(get_closure_body(fn_node), &new_ctx); // TODO: eval_program()
}

static Node *apply_primitive(Node *fn_node, Node *arglist, Context *ctx) {
  // TODO: validate arglist is correct for type
  const Primitive *prim = get_prim_op(fn_node);
  return prim->fn(arglist, ctx);
}

Node *eval_apply(Node *list, Context *ctx) {
  Node *fn_node = FIRST(list);
  Node *arglist = REST(list);

  print(fn_node, ctx);
  print(arglist, ctx);

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

Node *eval_closure(Node *list, Context *ctx) {
  if (!is_list(FIRST(list))) {
    raise(ERR_INVALID_ARG, __func__);
    return NULL;
  }
  return cons_closure(CTX_POOL(ctx), FIRST(list), FIRST(REST(list)),
                      CTX_ENV(ctx));
}

Node *eval_cons(Node *list, Context *ctx) {
  return CONS(FIRST(list), FIRST(REST(list)), ctx);
}

Node *eval_eq(Node *list, Context *ctx) {
  return (type(FIRST(list))->eq_fn(FIRST(list), FIRST(REST(list))))
             ? get_true(ctx)
             : get_nil(ctx);
}

Node *first(Node *list, Context *ctx) {
  (void)ctx;
  if (!is_list(FIRST(list))) {
    raise(ERR_INVALID_ARG, __func__);
    return NULL;
  }
  return FIRST(FIRST(list));
}

Node *eval_if(Node *list, Context *ctx) {
  Node *_bool = FIRST(list);
  Node *then = FIRST(REST(list));
  Node *_else = FIRST(REST(REST(list)));

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
  if (!is_list(FIRST(list))) {
    raise(ERR_INVALID_ARG, __func__);
    return NULL;
  }
  return cons_integer(CTX_POOL(ctx), _length(FIRST(list)));
}

Node *list(Node *car, Node *cdr, Context *ctx) {
  Node *empty = CONS(NULL, NULL, ctx);
  return CONS(car, CONS(cdr, empty, ctx), ctx);
  ;
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

static Node *pair(Node *list1, Node *list2, Context *ctx) {
  if (is_empty_list(list1) || is_empty_list(list2))
    return CONS(NULL, NULL, ctx);

  Node *first_pair = list(FIRST(list1), FIRST(list2), ctx);
  Node *rest_pairs = pair(REST(list1), REST(list2), ctx);

  return CONS(first_pair, rest_pairs, ctx);
}

Node *eval_pair(Node *list, Context *ctx) {
  if (!is_list(FIRST(list)) || !is_list(FIRST(REST(list)))) {
    raise(ERR_INVALID_ARG, __func__);
    return NULL;
  }
  return pair(FIRST(list), FIRST(REST(list)), ctx);
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
  if (!is_list(FIRST(list))) {
    raise(ERR_INVALID_ARG, __func__);
    return NULL;
  }
  Node *cdr = REST(FIRST(list));
  return is_list(cdr) ? cdr : CONS(cdr, NULL, ctx);
}

static Node *set(Node *car, Node *cdr, Context *ctx) {
  if (!is_symbol(car)) {
    raise(ERR_INVALID_ARG, __func__);
    return NULL;
  }
  env_set(CTX_ENV(ctx), get_symbol(car), cdr); // TODO: error handling
  return cdr;
}

Node *eval_set(Node *list, Context *ctx) {
  if (!is_symbol(FIRST(list))) {
    raise(ERR_INVALID_ARG, __func__);
    return NULL;
  }
  return set(FIRST(list), FIRST(REST(list)), ctx);
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

    Node *fn_node = eval(FIRST(expr), ctx);

    if (PRIMITIVE(QUOTE) == get_prim_op(fn_node)) {
      return FIRST(REST(expr));
    }

    Node *result = eval_list(REST(expr), ctx);

    Node *arglist = (PRIMITIVE(APPLY) == get_prim_op(fn_node))
               ? CONS(FIRST(result), REST(result), ctx)
               : CONS(fn_node, result, ctx);

    print(arglist, ctx);

    return eval_apply(arglist, ctx);

    // return (PRIMITIVE(APPLY) == get_prim_op(fn_node))
    //            ? apply(first(arglist, ctx), rest(arglist, ctx), ctx)
    //            : apply(fn_node, arglist, ctx);
  }

  raise(ERR_INTERNAL, DEBUG_LOCATION);
  return NULL;
}

Node *eval_list(Node *list, Context *ctx) {
  if (is_empty_list(list))
    return empty_list(ctx);

  Node *car = eval(FIRST(list), ctx);
  Node *cdr = eval_list(REST(list), ctx);

  return CONS(car, cdr, ctx);
}

Node *eval_program(Node *program, Context *ctx) {
  Node *result = NULL;

  for (Node *expr = FIRST(program); !is_empty_list(expr);
       expr = FIRST(program)) {

    result = eval(expr, ctx);
    program = REST(program);
  }

  return result;
}
