#include <assert.h>

#include "clsr.h"

Obj *apply(Obj *obj, EvalContext *ctx) {
  (void)obj; // FIXME: undo
  (void)ctx; // FIXME: undo
  return NULL;
}

Obj *closure(Obj *obj, EvalContext *ctx) {
  assert(OBJ_ISKIND(obj, Obj_Closure));

  obj_fprintf(stderr, obj);
  fprintf(stderr, "\n");

  ObjClosure *clsr = OBJ_AS_PTR(obj, closure);
  clsr->env = ctx->env;
  PUSH(ctx->stack, obj);
  return TRUE;
}

Obj *lookup(Obj *obj, EvalContext *ctx) {
  (void)obj;

  Obj *key = POP(ctx->stack);

  if (!(key && OBJ_ISKIND(key, Obj_Literal) &&
        OBJ_AS(key, literal).kind == Literal_Sym)) {
    return FALSE; // TODO
  }

  void *rval;

  if (env_lookup(ctx->env, OBJ_AS(key, literal).symbol, &rval))
    PUSH(ctx->stack, NULL); // TODO
  else
    PUSH(ctx->stack, rval);

  return TRUE;
}

Obj *push(Obj *obj, EvalContext *ctx) {
  (void)ctx; // only consumes

  ObjCall obj_call = OBJ_AS(obj, call);
  ObjList list = OBJ_AS(obj_call.args, list);

  for (unsigned int i = 0; i < list.count; ++i) {
    assert(OBJ_ISKIND(list.nodes[i], Obj_Literal));
    PUSH(ctx->stack, list.nodes[i]);
  }

  return TRUE;
}

/* a/k/a RETURN */
Obj *ret(Obj *obj, EvalContext *ctx) {
  (void)obj;
  (void)ctx;

  Obj *obj_rval = POP(ctx->stack);
  EXIT_FRAME(ctx->stack);
  PUSH(ctx->stack, obj_rval);

  return TRUE;
}

Obj *set(Obj *obj, EvalContext *ctx) {
  (void)obj;

  Obj *val = POP(ctx->stack);
  Obj *key = POP(ctx->stack);

  if (!(key && OBJ_ISKIND(key, Obj_Literal) &&
        OBJ_AS(key, literal).kind == Literal_Sym)) {
    return FALSE; // TODO
  }

  env_set(ctx->env, OBJ_AS(key, literal).symbol, val); // TODO
  return TRUE;
}

Obj *eval(Obj *obj, EvalContext *ctx) {
  if (obj == NULL) {
    return FALSE;
  }

  ObjList expressions = OBJ_AS(obj, list);

  for (unsigned int i = 0; i < expressions.count; ++i) {
    Obj *expression = expressions.nodes[i];

    if (OBJ_KIND(expression) == Obj_Call) {
      return OBJ_AS(expression, call).prim->prim_func(expression, ctx);
    } else if (OBJ_KIND(expression) == Obj_Closure) { // FIXME
      return closure(expression, ctx);
    } else {
      die("Unknown to eval\n");
    }
  }
  return FALSE;
}
