#include <assert.h>

#include "clsr.h"

extern Obj *const obj_true;
extern Obj *const obj_false;

Obj *apply(Obj *void_obj, EvalContext *ctx) {
  (void)void_obj;

  Obj *obj = POP(ctx->stack);

  assert(obj);
  assert(OBJ_ISKIND(obj, Obj_Closure));

  if (!obj || !(OBJ_ISKIND(obj, Obj_Closure))) {
    return obj_false; // TODO: error message
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

  return eval(obj_closure.body,
              &new_ctx); // does not EXIT_FRAME (ie force RETURN)
}

Obj *closure(Obj *obj, EvalContext *ctx) {
  assert(obj);
  assert(OBJ_ISKIND(obj, Obj_Closure));

  if (!obj || !(OBJ_ISKIND(obj, Obj_Closure))) {
    return obj_false; // TODO: error handling
  }

  ObjClosure *clsr = OBJ_AS_PTR(obj, closure);
  clsr->env = ctx->env;
  PUSH(ctx->stack, obj);
  return obj_true;
}

Obj *lookup(Obj *void_obj, EvalContext *ctx) {
  (void)void_obj;

  Obj *key = POP(ctx->stack);

  assert(key);
  assert(OBJ_ISKIND(key, Obj_Literal));
  assert(OBJ_AS(key, literal).kind == Literal_Sym);

  if (!key || !(OBJ_ISKIND(key, Obj_Literal)) ||
      !(OBJ_AS(key, literal).kind == Literal_Sym)) {
    return obj_false; // TODO: error handling
  }

  void *rval;

  if (env_lookup(ctx->env, OBJ_AS(key, literal).symbol, &rval))
    PUSH(ctx->stack, obj_false);
  else
    PUSH(ctx->stack, rval);

  return obj_true;
}

Obj *push(Obj *obj, EvalContext *ctx) {
  ObjCall obj_call = OBJ_AS(obj, call);
  ObjList list = OBJ_AS(obj_call.args, list);

  for (unsigned int i = list.count; i > 0; --i) {
    assert(OBJ_ISKIND(list.nodes[i - 1], Obj_Literal));
    PUSH(ctx->stack, list.nodes[i - 1]);
  }

  return obj_true;
}

Obj *return_(Obj *void_obj, EvalContext *ctx) {
  (void)void_obj;

  Obj *obj_rval = POP(ctx->stack);
  EXIT_FRAME(ctx->stack);
  PUSH(ctx->stack, obj_rval);

  return obj_true;
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
    return obj_false; // TODO: error message
  }

  env_set(ctx->env, OBJ_AS(key, literal).symbol, val); // TODO: error handling
  return obj_true;
}

Obj *eval(Obj *obj, EvalContext *ctx) {
  Obj *result = obj_false;

  if (obj == NULL) {
    return obj_false;
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
      return obj_false;
    }
  }
  return result;
}
