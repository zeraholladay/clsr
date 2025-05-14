#include <assert.h>
#include <stdio.h>

#include "core_def.h"
#include "debug.h"
#include "error.h"
#include "eval.h"
#include "prim_fn.h"
#include "safe_str.h"

#define NIL_STR "NIL"
#define T_STR   "T"

#define T   (&t_node)
#define NIL (&nil_node)

static Node nil_node = {.type    = TYPE_NIL,
                        .as.list = {.first = NULL, .rest = NULL}};

static Node t_node = {.type = TYPE_SYMBOL, .as.symbol = T_STR};

static Node *apply(Node *fn, Node *expr, Context *ctx);
static Node *funcall_closure(Node *fn, Node *args, Context *ctx);
static Node *funcall_primitive_fn(Node *fn, Node *args, Context *ctx);
static size_t length(Node *list);
static Node *lookup_symbol(Node *node, Context *ctx);
static Node *pair(Node *l1, Node *l2, Context *ctx);
static Node *set(Node *car, Node *cdr, Context *ctx);

static Node *apply(Node *fn, Node *expr, Context *ctx) {
  const PrimitiveFn *prim = GET_PRIMITIVE_FN(fn);
  Node *call_args         = NULL;

  if (prim == PRIM_FN(FUNCALL) || prim == PRIM_FN(APPLY)) {
    // (funcall f arg1 arg2 ...)
    // (apply f arglist)
    Node *fn2       = eval(FIRST(REST(expr)), ctx);
    Node *rest_args = (prim == PRIM_FN(FUNCALL))
                          ? eval_list(REST(REST(expr)), ctx)
                          : eval(FIRST(REST(REST(expr))), ctx);
    call_args       = CONS(fn2, rest_args, ctx);
  } else {
    // (f arg1 arg2 ... )
    Node *rest_args = eval_list(REST(expr), ctx);
    call_args       = CONS(fn, rest_args, ctx);
  }

  return eval_apply(call_args, ctx);
}

static Node *funcall_closure(Node *fn, Node *args, Context *ctx) {
  size_t expected = length(GET_CLOSURE_PARAMS(fn));
  size_t received = length(args);

  if (expected != received) {
    ErrorCode err =
        (received < expected) ? ERR_MISSING_ARG : ERR_UNEXPECTED_ARG;
    raise(err, __func__); // FIXME: context
    return NULL;
  }

  Context new_ctx   = *ctx;
  CTX_ENV(&new_ctx) = env_new(GET_CLOSURE_ENV(fn));

  // clang-format off
  for (Node *pairs = pair(GET_CLOSURE_PARAMS(fn), args, ctx);
       !IS_EMPTY_LIST(pairs); pairs = REST(pairs)) {

    Node *pair = FIRST(pairs);
    set(FIRST(pair), FIRST(REST(pair)), &new_ctx);
  }
  // clang-format on

  return eval(GET_CLOSURE_BODY(fn), &new_ctx); // TODO: eval_program()
}

static Node *funcall_primitive_fn(Node *fn, Node *args, Context *ctx) {
  const PrimitiveFn *prim_fn = GET_PRIMITIVE_FN(fn);
  int received               = (int)length(args);

  if (prim_fn->arity > 0 && prim_fn->arity != received) {
    ErrorCode err =
        (received < prim_fn->arity) ? ERR_MISSING_ARG : ERR_UNEXPECTED_ARG;
    raise(err, __func__); // FIXME: context
    return NULL;
  }

  return (prim_fn != PRIM_FN(LIST)) ? prim_fn->fn(args, ctx) : args;
}

static size_t length(Node *list) {
  size_t i = 0;
  for (Node *cdr = REST(list); cdr; cdr = REST(cdr))
    ++i;
  return i;
}

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
    if (!safe_strncmp_minlen(symstr, table[i].name, table[i].len)) {
      return table[i].value;
    }
  }

  return NULL;
}

static Node *lookup_symbol(Node *node, Context *ctx) {
  if (!IS_SYMBOL(node)) {
    raise(ERR_INTERNAL, __func__);
    return NULL;
  }

  const char *symstr = GET_SYMBOL(node);

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
  if (IS_EMPTY_LIST(list1) || IS_EMPTY_LIST(list2))
    return CONS(NULL, NULL, ctx);

  Node *first_pair = LIST(FIRST(list1), FIRST(list2), ctx);
  Node *rest_pairs = pair(REST(list1), REST(list2), ctx);

  return CONS(first_pair, rest_pairs, ctx);
}

static Node *set(Node *car, Node *cdr, Context *ctx) {
  if (!IS_SYMBOL(car)) {
    raise(ERR_INVALID_ARG, __func__);
    return NULL;
  }

  const char *symstr = GET_SYMBOL(car);
  size_t len         = strlen(symstr);

  if (prim_fn_lookup(symstr, len) || lookup_reserved_symbol(symstr)) {
    raise(ERR_INVALID_ARG, __func__);
    return NULL;
  }

  env_set(CTX_ENV(ctx), symstr, cdr); // TODO: error handling
  return cdr;
}

Node *eval_apply(Node *expr, Context *ctx) { return eval_funcall(expr, ctx); }

Node *eval_closure(Node *args, Context *ctx) {
  if (!IS_LIST(FIRST(args))) {
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
  if (!IS_LIST(FIRST(args))) {
    raise(ERR_INVALID_ARG, __func__);
    return NULL;
  }
  return FIRST(FIRST(args));
}

Node *eval_funcall(Node *args, Context *ctx) {
  Node *fn_node = FIRST(args);
  Node *arglist = REST(args);

  if (!(IS_CLOSURE(fn_node) || IS_PRIMITIVE_FN(fn_node)) || !IS_LIST(arglist)) {
    raise(ERR_INVALID_ARG, __func__);
    return NULL;
  }

  if (IS_CLOSURE(fn_node)) {
    return funcall_closure(fn_node, arglist, ctx);
  }

  if (IS_PRIMITIVE_FN(fn_node)) {
    return funcall_primitive_fn(fn_node, arglist, ctx);
  }

  raise(ERR_INTERNAL, DEBUG_LOCATION);
  return NULL;
}

Node *eval_if(Node *args, Context *ctx) {
  Node *condition  = FIRST(args);
  Node *true_expr  = FIRST(REST(args));
  Node *false_expr = FIRST(REST(REST(args)));

  Node *branch = type(T)->eq_fn(condition, T) ? true_expr : false_expr;
  return eval(branch, ctx);
}

Node *eval_len(Node *args, Context *ctx) {
  if (!IS_LIST(FIRST(args))) {
    raise(ERR_INVALID_ARG, __func__);
    return NULL;
  }
  return cons_integer(CTX_POOL(ctx), length(FIRST(args)));
}

Node *eval_list(Node *args, Context *ctx) {
  if (IS_EMPTY_LIST(args))
    return CONS(NULL, NULL, ctx);

  Node *car = eval(FIRST(args), ctx);
  Node *cdr = eval_list(REST(args), ctx);

  return CONS(car, cdr, ctx);
}

Node *eval_pair(Node *args, Context *ctx) {
  if (!IS_LIST(FIRST(args)) || !IS_LIST(FIRST(REST(args)))) {
    raise(ERR_INVALID_ARG, __func__);
    return NULL;
  }
  return pair(FIRST(args), FIRST(REST(args)), ctx);
}

Node *eval_print(Node *args, Context *ctx) {
  (void)ctx;
  char *str = type(args)->str_fn(args);
  printf("%s\n", str);
  free(str);
  return T;
}

Node *eval_rest(Node *args, Context *ctx) {
  (void)ctx;
  if (!IS_LIST(FIRST(args))) {
    raise(ERR_INVALID_ARG, __func__);
    return NULL;
  }
  Node *cdr = REST(FIRST(args));
  return IS_LIST(cdr) ? cdr : CONS(cdr, NULL, ctx);
}

Node *eval_set(Node *args, Context *ctx) {
  if (!IS_SYMBOL(FIRST(args))) {
    raise(ERR_INVALID_ARG, __func__);
    return NULL;
  }
  return set(FIRST(args), FIRST(REST(args)), ctx);
}

Node *eval_str(Node *args, Context *ctx) {
  return cons_string(CTX_POOL(ctx), type(args)->str_fn(args));
}

Node *eval(Node *expr, Context *ctx) {
  if (IS_SYMBOL(expr))
    return lookup_symbol(expr, ctx);

  if (!IS_LIST(expr))
    return expr;

  if (IS_LIST(expr)) {
    if (IS_EMPTY_LIST(expr)) {
      return expr;
    }

    Node *fn                = eval(FIRST(expr), ctx);
    const PrimitiveFn *prim = GET_PRIMITIVE_FN(fn);

    if (prim == PRIM_FN(QUOTE)) {
      return FIRST(REST(expr));
    }

    if (prim == PRIM_FN(EVAL)) {
      return eval(eval(FIRST(REST(expr)), ctx), ctx);
    }

    return apply(fn, expr, ctx);
  }

  raise(ERR_INTERNAL, DEBUG_LOCATION);
  return NULL;
}

Node *eval_program(Node *program, Context *ctx) {
  Node *result = NULL;

  for (Node *expr = FIRST(program); !IS_EMPTY_LIST(expr);
       expr       = FIRST(program)) {

    result  = eval(expr, ctx);
    program = REST(program);
  }

  return result;
}
