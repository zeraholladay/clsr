#include "error.h"
#include "eval.h"

// TODO: check for over/underflow someday

Node *
eval_add (Node *args, Context *ctx)
{
  if (!IS_LIST (args))
    {
      raise (ERR_ARG_NOT_ITERABLE, "add: argument is not a list");
      return NULL;
    }

  if (IS_EMPTY_LIST (args))
    {
      raise (ERR_INVALID_ARG_LENGTH, "add: expected >= 1 arguments");
      return NULL;
    }

  if (!IS_INTEGER (FIRST (args)))
    {
      raise (ERR_ARG_TYPE_MISMATCH, "add: argument is not integer");
      return NULL;
    }

  Integer sum = GET_INTEGER (FIRST (args));

  for (Node *cur = REST (args); !IS_EMPTY_LIST (cur); cur = REST (cur))
    {
      if (!IS_INTEGER (FIRST (cur)))
        {
          raise (ERR_ARG_TYPE_MISMATCH, "add: argument is not integer");
          return NULL;
        }

      sum += GET_INTEGER (FIRST (cur));
    }

  return cons_integer (CTX_POOL (ctx), sum);
}

Node *
eval_sub (Node *args, Context *ctx)
{
  if (!IS_LIST (args))
    {
      raise (ERR_ARG_NOT_ITERABLE, "sub: argument is not a list");
      return NULL;
    }

  if (IS_EMPTY_LIST (args))
    {
      raise (ERR_INVALID_ARG_LENGTH, "sub: expected >= 1 arguments");
      return NULL;
    }

  if (!IS_INTEGER (FIRST (args)))
    {
      raise (ERR_ARG_TYPE_MISMATCH, "sub: argument is not integer");
      return NULL;
    }

  Integer total = GET_INTEGER (FIRST (args));

  for (Node *cur = REST (args); !IS_EMPTY_LIST (cur); cur = REST (cur))
    {
      if (!IS_INTEGER (FIRST (cur)))
        {
          raise (ERR_ARG_TYPE_MISMATCH, "sub: argument is not integer");
          return NULL;
        }

      total -= GET_INTEGER (FIRST (cur));
    }

  return cons_integer (CTX_POOL (ctx), total);
}

Node *
eval_mul (Node *args, Context *ctx)
{
  if (!IS_LIST (args))
    {
      raise (ERR_ARG_NOT_ITERABLE, "mul: argument is not a list");
      return NULL;
    }

  if (IS_EMPTY_LIST (args))
    {
      raise (ERR_INVALID_ARG_LENGTH, "mul: expected >= 1 arguments");
      return NULL;
    }

  if (!IS_INTEGER (FIRST (args)))
    {
      raise (ERR_ARG_TYPE_MISMATCH, "mul: argument is not integer");
      return NULL;
    }

  Integer result = GET_INTEGER (FIRST (args));

  for (Node *cur = REST (args); !IS_EMPTY_LIST (cur); cur = REST (cur))
    {
      if (!IS_INTEGER (FIRST (cur)))
        {
          raise (ERR_ARG_TYPE_MISMATCH, "mul: argument is not integer");
          return NULL;
        }

      result *= GET_INTEGER (FIRST (cur));
    }

  return cons_integer (CTX_POOL (ctx), result);
}

Node *
eval_div (Node *args, Context *ctx)
{
  if (!IS_LIST (args))
    {
      raise (ERR_ARG_NOT_ITERABLE, "div: argument is not a list");
      return NULL;
    }

  if (IS_EMPTY_LIST (args))
    {
      raise (ERR_INVALID_ARG_LENGTH, "div: expected >= 1 arguments");
      return NULL;
    }

  if (!IS_INTEGER (FIRST (args)))
    {
      raise (ERR_ARG_TYPE_MISMATCH, "div: argument is not integer");
      return NULL;
    }

  Integer result = GET_INTEGER (FIRST (args));

  for (Node *cur = REST (args); !IS_EMPTY_LIST (cur); cur = REST (cur))
    {
      if (!IS_INTEGER (FIRST (cur)))
        {
          raise (ERR_ARG_TYPE_MISMATCH, "div: argument is not integer");
          return NULL;
        }

      if (GET_INTEGER (FIRST (cur)) == 0)
        {
          raise (ERR_DIVISION_BY_0, "div: argument is zero");
          return NULL;
        }

      result /= GET_INTEGER (FIRST (cur));
    }

  return cons_integer (CTX_POOL (ctx), result);
}
