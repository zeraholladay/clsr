#include "error.h"
#include "eval.h"

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
