#include <assert.h>
#include <stdio.h>

#include "debug.h"
#include "error.h"
#include "eval.h"
#include "keywords.h"
#include "safe_str.h"
#include "types.h"

#define NIL_STR "NIL"
#define T_STR "T"

#define T (&t_node)
#define NIL (&nil_node)

static Node nil_node
    = { .type = TYPE_NIL, .as.list = { .first = NULL, .rest = NULL } };

static Node t_node = { .type = TYPE_SYMBOL, .as.symbol = T_STR };

static Node *apply (Node *fn, Node *expr, Context *ctx);
static Node *funcall (Node *fn, Node *arglist, Context *ctx);
static Node *funcall_builtin (Node *fn, Node *args, Context *ctx);
static Node *funcall_lambda (Node *fn, Node *args, Context *ctx);
static size_t length (Node *list);
static Node *lookup_sym (Node *node, Context *ctx);
static Node *pair (Node *l1, Node *l2, Context *ctx);
static Node *set (Node *car, Node *cdr, Context *ctx);

static Node *
apply (Node *fn, Node *expr, Context *ctx)
{
  const Primitive *prim = GET_PRIMITIVE (fn);

  // (funcall f arg1 arg2 ...)
  if (prim == KEYWORD (FUNCALL))
    {
      Node *fn2 = eval (FIRST (REST (expr)), ctx);
      Node *rest_args = eval_list (REST (REST (expr)), ctx);

      return funcall (fn2, rest_args, ctx);
    }

  // (apply f arglist)
  if (prim == KEYWORD (APPLY))
    {
      Node *fn2 = eval (FIRST (REST (expr)), ctx);
      Node *rest_args = eval (FIRST (REST (REST (expr))), ctx);

      return funcall (fn2, rest_args, ctx);
    }

  // (fun arg1 arg2 ... )
  Node *rest_args = eval_list (REST (expr), ctx);
  return funcall (fn, rest_args, ctx);
}

static Node *
funcall (Node *fn, Node *arglist, Context *ctx)
{
  if (IS_PRIMITIVE (fn))
    {
      return funcall_builtin (fn, arglist, ctx);
    }

  if (IS_LAMBDA (fn))
    {
      return funcall_lambda (fn, arglist, ctx);
    }

  raise (ERR_INTERNAL, DEBUG_LOCATION);
  return NULL;
}

static Node *
funcall_builtin (Node *fn, Node *arglist, Context *ctx)
{
  const Primitive *prim = GET_PRIMITIVE (fn);
  int received = (int)length (arglist);

  if (prim->arity > 0 && prim->arity != received)
    {
      ErrorCode err
          = (received < prim->arity) ? ERR_MISSING_ARG : ERR_UNEXPECTED_ARG;
      raise (err, prim->name);
      return NULL;
    }

  return (prim != KEYWORD (LIST)) ? prim->fn (arglist, ctx) : arglist;
}

static Node *
funcall_lambda (Node *fn, Node *args, Context *ctx)
{
  size_t expected = length (GET_LAMBDA_PARAMS (fn));
  size_t received = length (args);

  if (expected != received)
    {
      ErrorCode err
          = (received < expected) ? ERR_MISSING_ARG : ERR_UNEXPECTED_ARG;
      raise (err, __func__);
      return NULL;
    }

  Context new_ctx = *ctx;
  CTX_ENV (&new_ctx) = env_new (GET_LAMBDA_ENV (fn));

  Node *pairs = pair (GET_LAMBDA_PARAMS (fn), args, ctx);

  while (!IS_EMPTY_LIST (pairs))
    {
      Node *pair = FIRST (pairs);
      set (FIRST (pair), FIRST (REST (pair)), &new_ctx);
      pairs = REST (pairs);
    }

  return eval_program (GET_LAMBDA_BODY (fn), &new_ctx);
}

static size_t
length (Node *list)
{
  size_t i = 0;

  for (Node *cdr = REST (list); cdr; cdr = REST (cdr))
    ++i;
    
  return i;
}

static Node *
lookup_reserved_sym (const char *symstr)
{
  static const struct
  {
    const char *name;
    size_t len;
    Node *value;
  } table[] = {
    { NIL_STR, sizeof (NIL_STR), NIL },
    { T_STR, sizeof (T_STR), T },
  };

  for (size_t i = 0; i < sizeof (table) / sizeof (*table); ++i)
    {
      if (!safe_strncmp_minlen (symstr, table[i].name, table[i].len))
        {
          return table[i].value;
        }
    }

  return NULL;
}

static Node *
lookup_sym (Node *node, Context *ctx)
{
  if (!IS_SYMBOL (node))
    {
      raise (ERR_INTERNAL, __func__);
      return NULL;
    }

  const char *symstr = GET_SYMBOL (node);

  Node *reserved = lookup_reserved_sym (symstr);

  if (reserved)
    return reserved;

  rb_node *n = env_lookup (CTX_ENV (ctx), symstr);

  if (!n)
    {
      raise (ERR_SYMBOL_NOT_FOUND, symstr);
      return NULL;
    }

  return RB_VAL (n);
}

static Node *
pair (Node *list1, Node *list2, Context *ctx)
{
  if (IS_EMPTY_LIST (list1) || IS_EMPTY_LIST (list2))
    return CONS (NULL, NULL, ctx);

  Node *first_pair = LIST2 (FIRST (list1), FIRST (list2), ctx);
  Node *rest_pairs = pair (REST (list1), REST (list2), ctx);

  return CONS (first_pair, rest_pairs, ctx);
}

static Node *
set (Node *first, Node *rest, Context *ctx)
{
  if (!IS_SYMBOL (first))
    {
      raise (ERR_INVALID_ARG, __func__);
      return NULL;
    }

  const char *symstr = GET_SYMBOL (first);
  size_t len = strlen (symstr);

  if (keyword_lookup (symstr, len) || lookup_reserved_sym (symstr))
    {
      raise (ERR_INVALID_ARG, __func__);
      return NULL;
    }

  env_set (CTX_ENV (ctx), symstr, rest); // TODO: error handling
  return rest;
}

Node *
eval_apply (Node *args, Context *ctx)
{
  return eval_funcall (args, ctx); // the real apply has alread run.
}

Node *
eval_cons (Node *args, Context *ctx)
{
  return CONS (FIRST (args), FIRST (REST (args)), ctx);
}

Node *
eval_eq (Node *args, Context *ctx)
{
  (void)ctx;
  Node *first = FIRST (args);
  Node *second = FIRST (REST (args));

  EqFn fn = type (first)->eq_fn;

  if (fn (FIRST (args), second))
    {
      return T;
    }

  return NIL;
}

Node *
eval_first (Node *args, Context *ctx)
{
  (void)ctx;
  if (!IS_LIST (FIRST (args)))
    {
      raise (ERR_INVALID_ARG, __func__);
      return NULL;
    }

  return FIRST (FIRST (args));
}

Node *
eval_funcall (Node *args, Context *ctx)
{
  Node *fn_node = FIRST (args);
  Node *arglist = REST (args);

  if (!(IS_LAMBDA (fn_node) || IS_PRIMITIVE (fn_node)) || !IS_LIST (arglist))
    {
      raise (ERR_INVALID_ARG, __func__);
      return NULL;
    }

  return funcall (fn_node, arglist, ctx);
}

Node *
eval_if (Node *args, Context *ctx)
{
  Node *condition = FIRST (args);
  Node *then_expr = FIRST (REST (args));
  Node *else_expr = FIRST (REST (REST (args)));

  EqFn eq_fn = type (T)->eq_fn;

  if (eq_fn (condition, T))
    {
      return eval (then_expr, ctx);
    }

  return eval (else_expr, ctx);
}

Node *
eval_len (Node *args, Context *ctx)
{
  Node *first = FIRST (args);

  if (!IS_LIST (args))
    {
      raise (ERR_INVALID_ARG, __func__);
      return NULL;
    }

  return cons_integer (CTX_POOL (ctx), length (first));
}

Node *
eval_list (Node *args, Context *ctx)
{
  if (IS_EMPTY_LIST (args))
    return CONS (NULL, NULL, ctx);

  Node *car = eval (FIRST (args), ctx);
  Node *cdr = eval_list (REST (args), ctx);

  return CONS (car, cdr, ctx);
}

Node *
eval_pair (Node *args, Context *ctx)
{
  if (!IS_LIST (FIRST (args)) || !IS_LIST (FIRST (REST (args))))
    {
      raise (ERR_INVALID_ARG, __func__);
      return NULL;
    }

  return pair (FIRST (args), FIRST (REST (args)), ctx);
}

Node *
eval_print (Node *args, Context *ctx)
{
  (void)ctx;

  if (!IS_LIST (args))
    {
      raise (ERR_INVALID_ARG, __func__);
      return NULL;
    }

  StrFn fn = type (FIRST (args))->str_fn;
  char *str = fn (FIRST (args));
  printf ("%s\n", str);
  free (str);
  return T;
}

Node *
eval_rest (Node *args, Context *ctx)
{
  (void)ctx;

  Node *first = FIRST (args);

  if (!IS_LIST (first))
    {
      raise (ERR_INVALID_ARG, __func__);
      return NULL;
    }

  Node *rest = REST (first);

  return rest ? rest : NIL;
}

Node *
eval_set (Node *args, Context *ctx)
{
  if (!IS_SYMBOL (FIRST (args)))
    {
      raise (ERR_INVALID_ARG, __func__);
      return NULL;
    }

  return set (FIRST (args), FIRST (REST (args)), ctx);
}

Node *
eval_str (Node *args, Context *ctx)
{
  return cons_string (CTX_POOL (ctx), type (args)->str_fn (args));
}

Node *
eval (Node *expr, Context *ctx)
{
  if (IS_SYMBOL (expr))
    return lookup_sym (expr, ctx);

  if (!IS_LIST (expr))
    return expr;

  if (IS_LIST (expr))
    {
      if (IS_EMPTY_LIST (expr))
        {
          return expr;
        }

      if (IS_LAMBDA (FIRST (expr)))
        {
          return FIRST (expr);
        }

      Node *fn = eval (FIRST (expr), ctx);
      const Primitive *prim = GET_PRIMITIVE (fn);

      if (prim == KEYWORD (QUOTE))
        {
          return FIRST (REST (expr));
        }

      if (prim == KEYWORD (EVAL))
        {
          return eval (eval (FIRST (REST (expr)), ctx), ctx);
        }

      return apply (fn, expr, ctx);
    }

  raise (ERR_INTERNAL, DEBUG_LOCATION);
  return NULL;
}

Node *
eval_program (Node *program, Context *ctx)
{
  Node *result = NULL;

  for (Node *expr = FIRST (program); !IS_EMPTY_LIST (expr);
       expr = FIRST (program))
    {

      result = eval (expr, ctx);
      program = REST (program);
    }

  return result;
}
