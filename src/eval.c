#include <assert.h>
#include <stdio.h>

#include "core_def.h"
#include "debug.h"
#include "error.h"
#include "eval.h"
#include "safe_str.h"

#define NIL_STR "NIL"
#define T_STR "T"

#define T (&t_node)
#define NIL (&nil_node)

static Node nil_node = {.type = TYPE_NIL,
                        .as.list = {.first = NULL, .rest = NULL}};

static Node t_node = {.type = TYPE_SYMBOL, .as.symbol = T_STR};

static size_t length(Node *list);
static Node *lookup_symbol(Node *node, Context *ctx);
static Node *pair(Node *list1, Node *list2, Context *ctx);
static Node *set(Node *car, Node *cdr, Context *ctx);

static Node *lookup_reserved_symbol(const char *symstr) {
  static const struct {
    const char *name;
    size_t len;
    Node *value;
  } table[] = {
      {NIL_STR, sizeof(NIL_STR), NIL},
      {T_STR, sizeof(T_STR), T},
  };

  for (size_t i = 0; i < sizeof(table) / sizeof(*table); ++i) {
    if (safe_strncmp_minlen(symstr, table[i].name, table[i].len) == 0) {
      return table[i].value;
    }
  }

  return NULL;
}

static Node *apply_closure(Node *fn, Node *args, Context *ctx) {
  size_t expected = length(get_closure_params(fn));
  size_t received = length(args);

  if (expected != received) {
    ErrorCode err =
        (received < expected) ? ERR_MISSING_ARG : ERR_UNEXPECTED_ARG;
    raise(err, __func__); // FIXME: context
    return NULL;
  }

  Context new_ctx = *ctx;
  CTX_ENV(&new_ctx) = env_new(get_closure_env(fn));

  for (Node *pairs = pair(get_closure_params(fn), args, ctx);
       !is_empty_list(pairs); pairs = REST(pairs)) {

    Node *pair = FIRST(pairs);
    set(FIRST(pair), FIRST(REST(pair)), &new_ctx);
  }

  return eval(get_closure_body(fn), &new_ctx); // TODO: eval_program()
}

static Node *apply_primitive(Node *fn, Node *args, Context *ctx) {
  // TODO: validate args is correct for type
  const Primitive *prim = get_prim_op(fn);
  return prim->fn(args, ctx);
}

Node *eval_apply(Node *list, Context *ctx) {
  Node *fn_node = FIRST(list);
  Node *arglist = REST(list);

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

Node *eval_closure(Node *args, Context *ctx) {
  if (!is_list(FIRST(args))) {
    raise(ERR_INVALID_ARG, __func__);
    return NULL;
  }
  return cons_closure(CTX_POOL(ctx), FIRST(args), FIRST(REST(args)),
                      CTX_ENV(ctx));
}

Node *eval_cons(Node *args, Context *ctx) {
  return CONS(FIRST(args), FIRST(REST(args)), ctx);
}

Node *eval_eq(Node *args, Context *ctx) {
  (void)ctx;
  return (type(FIRST(args))->eq_fn(FIRST(args), FIRST(REST(args)))) ? T : NIL;
}

Node *eval_first(Node *args, Context *ctx) {
  (void)ctx;
  if (!is_list(FIRST(args))) {
    raise(ERR_INVALID_ARG, __func__);
    return NULL;
  }
  return FIRST(FIRST(args));
}

Node *eval_if(Node *args, Context *ctx) {
  Node *condition = FIRST(args);
  Node *true_expr = FIRST(REST(args));
  Node *false_expr = FIRST(REST(REST(args)));

  Node *branch = type(T)->eq_fn(condition, T) ? true_expr : false_expr;
  return eval(branch, ctx);
}

static size_t length(Node *list) {
  size_t i = 0;
  for (Node *cdr = REST(list); cdr; cdr = REST(cdr))
    ++i;
  return i;
}

Node *eval_len(Node *args, Context *ctx) {
  if (!is_list(FIRST(args))) {
    raise(ERR_INVALID_ARG, __func__);
    return NULL;
  }
  return cons_integer(CTX_POOL(ctx), length(FIRST(args)));
}

// TODO:
Node *list(Node *car, Node *cdr, Context *ctx) {
  Node *empty = CONS(NULL, NULL, ctx);
  return CONS(car, CONS(cdr, empty, ctx), ctx);
  ;
}

static Node *lookup_symbol(Node *node, Context *ctx) {
  if (!is_symbol(node)) {
    raise(ERR_INTERNAL, __func__);
    return NULL;
  }

  const char *symstr = get_symbol(node);

  Node *reserved = lookup_reserved_symbol(symstr);

  if (reserved)
    return reserved;

  rb_node *n = env_lookup(CTX_ENV(ctx), symstr);

  if (!n) {
    raise(ERR_SYMBOL_NOT_FOUND, symstr);
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

Node *eval_pair(Node *args, Context *ctx) {
  if (!is_list(FIRST(args)) || !is_list(FIRST(REST(args)))) {
    raise(ERR_INVALID_ARG, __func__);
    return NULL;
  }
  return pair(FIRST(args), FIRST(REST(args)), ctx);
}

Node *eval_print(Node *node, Context *ctx) {
  (void)ctx;
  char *str = type(node)->str_fn(node);
  printf("%s\n", str);
  free(str);
  return T;
}

Node *eval_rest(Node *args, Context *ctx) {
  (void)ctx;
  if (!is_list(FIRST(args))) {
    raise(ERR_INVALID_ARG, __func__);
    return NULL;
  }
  Node *cdr = REST(FIRST(args));
  return is_list(cdr) ? cdr : CONS(cdr, NULL, ctx);
}

static Node *set(Node *car, Node *cdr, Context *ctx) {
  if (!is_symbol(car)) {
    raise(ERR_INVALID_ARG, __func__);
    return NULL;
  }

  const char *symstr = get_symbol(car);
  size_t len = strlen(symstr);

  if (primitive_lookup(symstr, len) || lookup_reserved_symbol(symstr)) {
    raise(ERR_INVALID_ARG, __func__);
    return NULL;
  }

  env_set(CTX_ENV(ctx), symstr, cdr); // TODO: error handling
  return cdr;
}

Node *eval_set(Node *args, Context *ctx) {
  if (!is_symbol(FIRST(args))) {
    raise(ERR_INVALID_ARG, __func__);
    return NULL;
  }
  return set(FIRST(args), FIRST(REST(args)), ctx);
}

Node *eval_str(Node *node, Context *ctx) {
  return cons_string(CTX_POOL(ctx), type(node)->str_fn(node));
}

Node *eval(Node *expr, Context *ctx) {
  if (is_symbol(expr))
    return lookup_symbol(expr, ctx);

  if (!is_list(expr))
    return expr;

  if (is_list(expr)) {
    if (is_empty_list(expr)) {
      return expr;
    }

    Node *fn = eval(FIRST(expr), ctx);

    if (PRIMITIVE(QUOTE) == get_prim_op(fn)) {
      return FIRST(REST(expr));
    }

    Node *args = (PRIMITIVE(APPLY) != get_prim_op(fn))
                     ? CONS(fn, eval_list(REST(expr), ctx), ctx)
                     : eval_list(REST(expr), ctx);

    return eval_apply(args, ctx);
  }

  raise(ERR_INTERNAL, DEBUG_LOCATION);
  return NULL;
}

Node *eval_list(Node *args, Context *ctx) {
  if (is_empty_list(args))
    return CONS(NULL, NULL, ctx);

  Node *car = eval(FIRST(args), ctx);
  Node *cdr = eval_list(REST(args), ctx);

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
