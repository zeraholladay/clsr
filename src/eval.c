#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "debug.h"
#include "error.h"
#include "eval.h"
#include "keywords.h"
#include "parser.h"
#include "safe_str.h"
#include "types.h"

#define PRINT(node)                                                           \
  do                                                                          \
    {                                                                         \
      StrFn to_str_fn = type (node)->str_fn;                                  \
      char *str = to_str_fn (node);                                           \
      printf ("%s\n", str);                                                   \
      free (str);                                                             \
    }                                                                         \
  while (0)

#define NIL_STR "NIL"
#define T_STR "T"

Node nil_node
    = { .type = TYPE_NIL, .as.list = { .first = NULL, .rest = NULL } };

Node t_node = { .type = TYPE_SYMBOL, .as.symbol = T_STR };

static Node *apply (Node *fn, Node *expr, Context *ctx);
static Node *funcall (Node *fn, Node *arglist, Context *ctx);
static Node *funcall_builtin (Node *fn, Node *args, Context *ctx);
static Node *funcall_lambda (Node *fn, Node *args, Context *ctx);
static size_t length (Node *list);
static Node *lookup (Node *node, Context *ctx);
static Node *pair (Node *l1, Node *l2, Context *ctx);
static Node *set (Node *car, Node *cdr, Context *ctx);

static Node *
apply (Node *fn, Node *expr, Context *ctx)
{
  const Primitive *prim = GET_PRIMITIVE (fn);

  if (prim == KEYWORD (FUNCALL))
    {
      // (funcall f arg1 arg2 ...)
      Node *fn2 = eval (FIRST (REST (expr)), ctx);
      Node *rest_args = eval_list (REST (REST (expr)), ctx);

      return funcall (fn2, rest_args, ctx);
    }
  else if (prim == KEYWORD (APPLY))
    {
      // (apply f arglist)
      Node *fn2 = eval (FIRST (REST (expr)), ctx);
      Node *rest_args = eval (FIRST (REST (REST (expr))), ctx);

      return funcall (fn2, rest_args, ctx);
    }
  else
    {
      // (fun arg1 arg2 ... )
      Node *rest_args = eval_list (REST (expr), ctx);

      return funcall (fn, rest_args, ctx);
    }
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

  raise (ERR_NOT_A_FUNCTION, type (fn)->str_fn (fn));
  return NULL;
}

static Node *
funcall_builtin (Node *fn, Node *arglist, Context *ctx)
{
  const Primitive *prim = GET_PRIMITIVE (fn);
  int received = (int)length (arglist);

  if (!IS_PRIMITIVE (fn) || !prim->fn || prim->token != INTRINSIC_PRIMITIVE)
    {
      raise (ERR_NOT_A_FUNCTION, type (fn)->str_fn (fn));
      return NULL;
    }

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

  while (!IS_NIL (pairs))
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
lookup (Node *node, Context *ctx)
{
  static const struct
  {
    const char *name;
    size_t len;
    Node *value;
  } kywd_tab[] = {
    { T_STR, sizeof (T_STR), T },
  };

  const size_t kywd_tab_len = sizeof (kywd_tab) / sizeof (*kywd_tab);

  if (!IS_SYMBOL (node))
    {
      raise (ERR_INTERNAL, __func__);
      return NULL;
    }

  const char *symstr = GET_SYMBOL (node);

  // Is it a singleton symbol?
  for (size_t i = 0; i < kywd_tab_len; ++i)
    {
      const char *name = kywd_tab[i].name;
      size_t len = kywd_tab[i].len;

      if (!safe_strncmp_minlen (symstr, name, len))
        {
          return kywd_tab[i].value;
        }
    }

  // else lookup in the env
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
  if (IS_NIL (list1) || IS_NIL (list2))
    return NIL;

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

  if (keyword_lookup (symstr, len))
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
eval_first (Node *args, Context *ctx)
{
  (void)ctx;
  if (!LISTP (FIRST (args)))
    {
      raise (ERR_INVALID_ARG, __func__);
      return NULL;
    }

  Node *first = FIRST (FIRST (args));

  return first ? first : NIL;
}

Node *
eval_funcall (Node *args, Context *ctx)
{
  Node *fn_node = FIRST (args);
  Node *arglist = REST (args);

  if (!(IS_LAMBDA (fn_node) || IS_PRIMITIVE (fn_node)) || !LISTP (arglist))
    {
      raise (ERR_INVALID_ARG, __func__);
      return NULL;
    }

  return funcall (fn_node, arglist, ctx);
}

Node *
eval_len (Node *args, Context *ctx)
{
  Node *first = FIRST (args);

  if (!LISTP (args))
    {
      raise (ERR_INVALID_ARG, __func__);
      return NULL;
    }

  return cons_integer (&CTX_POOL (ctx), length (first));
}

Node *
eval_list (Node *args, Context *ctx)
{
  if (IS_NIL (args))
    return NIL;

  Node *car = eval (FIRST (args), ctx);
  Node *cdr = eval_list (REST (args), ctx);

  return CONS (car, cdr, ctx);
}

Node *
eval_pair (Node *args, Context *ctx)
{
  if (!LISTP (FIRST (args)) || !LISTP (FIRST (REST (args))))
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

  if (!LISTP (args))
    {
      raise (ERR_INVALID_ARG, __func__);
      return NULL;
    }

  PRINT (FIRST (args));
  return T;
}

Node *
eval_rest (Node *args, Context *ctx)
{
  (void)ctx;

  Node *first = FIRST (args);

  if (!LISTP (first))
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
  return cons_string (&CTX_POOL (ctx), type (args)->str_fn (args));
}

Node *
eval (Node *expr, Context *ctx)
{
  // SYMBOLS
  if (IS_SYMBOL (expr))
    return lookup (expr, ctx);

  // LITERALS: NUMBERS, STRINGS, ETC.
  if (!LISTP (expr))
    return expr;

  if (LISTP (expr))
    {

      // NIL
      if (IS_NIL (expr))
        {
          return NIL;
        }

      // LAMBDA
      if (IS_LAMBDA (FIRST (expr)))
        {
          return FIRST (expr);
        }

      Node *fn = eval (FIRST (expr), ctx);
      const Primitive *prim = GET_PRIMITIVE (fn);

      // QUOTE
      if (IS_PRIMITIVE (fn) && prim->token == QUOTE_PRIMITIVE)
        {
          return FIRST (REST (expr));
        }

      // EVAL
      if (IS_PRIMITIVE (fn) && prim->token == EVAL_PRIMITIVE)
        {
          return eval (eval (FIRST (REST (expr)), ctx), ctx);
        }

      // IF
      if (IS_PRIMITIVE (fn) && prim->token == IF_PRIMITIVE)
        {
          Node *pred_expr = FIRST (REST (expr));

          if (!IS_NIL (eval (pred_expr, ctx)))
            {
              return eval (FIRST (REST (REST (expr))), ctx);
            }
          else
            {
              return eval (FIRST (REST (REST (REST (expr)))), ctx);
            }
        }

      return apply (fn, expr, ctx);
    }

  raise (ERR_INTERNAL, DEBUG_LOCATION);
  return NULL;
}

Node *
eval_program (Node *program, Context *ctx)
{
  Node *result = NIL;

  do
    {
      result = eval (FIRST (program), ctx);
      program = REST (program);
    }
  while (!IS_NIL (program));

  return result;
}
