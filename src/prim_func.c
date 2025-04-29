#include <assert.h>

#include "clsr.h"

Obj *apply(Obj *void_obj, EvalContext *ctx) {
  (void)void_obj;

  Obj *obj = POP(ctx->stack);

  assert(obj);
  assert(OBJ_ISKIND(obj, Obj_Closure));

  if (!obj || !(OBJ_ISKIND(obj, Obj_Closure))) {
    return FALSE; // TODO: error handling
  }

  ObjClosure obj_closure = OBJ_AS(obj, closure);

  ObjList params = OBJ_AS(obj_closure.params, list);

  Env *closure_env = env_new(obj_closure.env); // TODO: error handling

  for (unsigned int i = 0; i < params.count; ++i) {
    Obj *o = POP(ctx->stack);
    const char *symbol = OBJ_AS(params.nodes[i], literal).symbol;
    env_set(closure_env, symbol, o);
  }

  ENTER_FRAME(ctx->stack);

  EvalContext new_ctx = {
      .stack = ctx->stack,
      .env = closure_env,
  };

  return eval(obj_closure.body, &new_ctx); // TODO: force EXIT_FRAME?
}

Obj *closure(Obj *obj, EvalContext *ctx) {
  assert(obj);
  assert(OBJ_ISKIND(obj, Obj_Closure));

  if (!obj || !(OBJ_ISKIND(obj, Obj_Closure))) {
    return FALSE; // TODO: error handling
  }

  ObjClosure *clsr = OBJ_AS_PTR(obj, closure);
  clsr->env = ctx->env;
  PUSH(ctx->stack, obj);
  return TRUE;
}

Obj *lookup(Obj *void_obj, EvalContext *ctx) {
  (void)void_obj;

  Obj *key = POP(ctx->stack);

  assert(key);
  assert(OBJ_ISKIND(key, Obj_Literal));
  assert(OBJ_AS(key, literal).kind == Literal_Sym);

  if (!key || !(OBJ_ISKIND(key, Obj_Literal)) ||
      !(OBJ_AS(key, literal).kind == Literal_Sym)) {
    return FALSE; // TODO: error handling
  }

  void *rval;

  if (env_lookup(ctx->env, OBJ_AS(key, literal).symbol, &rval))
    PUSH(ctx->stack, NULL); // TODO: what happens if the symbol isn't found?
  else
    PUSH(ctx->stack, rval);

  return TRUE;
}

Obj *push(Obj *obj, EvalContext *ctx) {
  ObjCall obj_call = OBJ_AS(obj, call);
  ObjList list = OBJ_AS(obj_call.args, list);

  for (unsigned int i = list.count; i > 0; --i) {
    assert(OBJ_ISKIND(list.nodes[i - 1], Obj_Literal));
    PUSH(ctx->stack, list.nodes[i - 1]);
  }

  return TRUE;
}

/* a/k/a RETURN */
Obj *ret(Obj *void_obj, EvalContext *ctx) {
  (void)void_obj;

  Obj *obj_rval = POP(ctx->stack);
  EXIT_FRAME(ctx->stack);
  PUSH(ctx->stack, obj_rval);

  return TRUE;
}

Obj *set(Obj *void_obj, EvalContext *ctx) {
  (void)void_obj;

  Obj *key = POP(ctx->stack);
  Obj *val = POP(ctx->stack);

  assert(key);
  assert(OBJ_ISKIND(key, Obj_Literal));
  assert(OBJ_AS(key, literal).kind == Literal_Sym);

  if (!key || !(OBJ_ISKIND(key, Obj_Literal)) ||
      !(OBJ_AS(key, literal).kind == Literal_Sym)) {
    return FALSE; // TODO: error handling
  }

  env_set(ctx->env, OBJ_AS(key, literal).symbol, val); // TODO
  return TRUE;
}

Obj *eval(Obj *obj, EvalContext *ctx) {
  Obj *result = FALSE;

  if (obj == NULL) {
    return FALSE;
  }

  assert(obj);
  assert(OBJ_ISKIND(obj, Obj_List));

  ObjList expressions = OBJ_AS(obj, list);

  for (unsigned int i = 0; i < expressions.count; ++i) {
    Obj *expression = expressions.nodes[i];

    if (OBJ_KIND(expression) == Obj_Call) {
      result = OBJ_AS(expression, call).prim->prim_func(expression, ctx);
    } else if (OBJ_KIND(expression) == Obj_Closure) { // FIXME
      result = closure(expression, ctx);
    } else {
      die("Unknown to eval\n");
      return FALSE;
    }
  }
  return result;
}
