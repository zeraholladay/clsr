#include <assert.h>

#include "clsr.h"

static Obj _obj_true = {.kind = Obj_Literal,
                        .as.literal = {
                            .kind = Literal_Keywrd,
                            .symbol = "True",
                        }};
static Obj _obj_false = {.kind = Obj_Literal,
                         .as.literal = {
                             .kind = Literal_Keywrd,
                             .symbol = "False",
                         }};

Obj *const obj_true = &_obj_true;
Obj *const obj_false = &_obj_false;

Obj *apply(Obj *void_obj, ClsrContext *ctx) {
  (void)void_obj;

  Obj *obj = CTX_POP(ctx);

  if (!OBJ_IS_CLOSURE(obj)) {
    assert(0 && "Obj is not a closure.");
    return obj_false; // TODO: error message
  }

  ObjClosure obj_closure = OBJ_AS(obj, closure);

  ObjList params = OBJ_AS(obj_closure.params, list);

  Env *closure_env = env_new(obj_closure.env); // TODO: error handling

  for (unsigned int i = 0; i < params.count; ++i) {
    Obj *o = CTX_POP(ctx);
    const char *symbol = OBJ_AS(params.nodes[i], literal).symbol;
    env_set(closure_env, symbol, o);
  }

  ENTER_FRAME(ctx->eval_ctx.stack);

  ClsrContext new_ctx = *ctx;
  new_ctx.eval_ctx.env = closure_env;

  return eval(obj_closure.body,
              &new_ctx); // does not EXIT_FRAME (ie force RETURN)
}

Obj *closure(Obj *obj, ClsrContext *ctx) {
  if (!OBJ_IS_CLOSURE(obj)) {
    assert(0 && "Obj is not a closure.");
    return obj_false; // TODO: error handling
  }

  ObjClosure *clsr = OBJ_AS_PTR(obj, closure);
  clsr->env = CTX_ENV(ctx);
  CTX_PUSH(ctx, obj);
  return obj_true;
}

Obj *eq(Obj *obj, ClsrContext *ctx) {
  Obj *obj1 = CTX_POP(ctx);
  Obj *obj2 = CTX_POP(ctx);

  // Literal ints are special
  if (OBJ_IS_LITERAL_INT(obj1) && OBJ_IS_LITERAL_INT(obj2)) {
    Obj *result = OBJ_AS(obj1, literal).integer == OBJ_AS(obj2, literal).integer
                      ? obj_true
                      : obj_false;
    CTX_PUSH(ctx, result);
    return obj_true;
  }

  // Symbols, keyword literals, lists, calls, closures, ifs, etc.
  // if we get lists, a comparison would be cool.
  CTX_PUSH(ctx, obj2);
  CTX_PUSH(ctx, obj1);
  return is(obj, ctx);
}

Obj *if_(Obj *obj, ClsrContext *ctx) {
  if (!OBJ_IS_IF(obj)) {
    assert(0 && "Obj is not an if.");
    return obj_false; // TODO: error handling
  }

  Obj *cond = CTX_POP(ctx);

  if (!cond) {
    return obj_false; // TODO: error handling
  }

  ObjIf obj_if = OBJ_AS(obj, if_);

  Obj *branch = (cond == obj_true) ? obj_if.then : obj_if.else_;

  return eval(branch, ctx);
}

Obj *is(Obj *void_obj, ClsrContext *ctx) {
  (void)void_obj;

  Obj *obj1 = CTX_POP(ctx);
  Obj *obj2 = CTX_POP(ctx);

  Obj *result = NULL;

  // symbols are globally unique
  if (OBJ_IS_LITERAL_SYM(obj1) && OBJ_IS_LITERAL_SYM(obj1)) {
    const char *sym1 = OBJ_AS(obj1, literal).symbol;
    const char *sym2 = OBJ_AS(obj2, literal).symbol;
    result = (uintptr_t)sym1 == (uintptr_t)sym2 ? obj_true : obj_false;
  } else
    result = (uintptr_t)obj1 == (uintptr_t)obj2 ? obj_true : obj_false;

  CTX_PUSH(ctx, result);

  return obj_true;
}

Obj *lookup(Obj *void_obj, ClsrContext *ctx) {
  (void)void_obj;

  Obj *key = CTX_POP(ctx);

  if (!OBJ_IS_LITERAL_SYM(key)) {
    assert(0 && "Obj is not a literal symbol.");
    return obj_false; // TODO: error handling
  }

  void *rval;

  if (env_lookup(CTX_ENV(ctx), OBJ_AS(key, literal).symbol, &rval))
    return obj_false;
  else
    CTX_PUSH(ctx, rval);

  return obj_true;
}

Obj *push(Obj *obj, ClsrContext *ctx) {
  ObjCall obj_call = OBJ_AS(obj, call);
  ObjList list = OBJ_AS(obj_call.args, list);

  for (unsigned int i = list.count; i > 0; --i) {
    assert(OBJ_ISKIND(list.nodes[i - 1], Obj_Literal));
    CTX_PUSH(ctx, list.nodes[i - 1]);
  }

  return obj_true;
}

Obj *return_(Obj *void_obj, ClsrContext *ctx) {
  (void)void_obj;

  Obj *obj_rval = CTX_POP(ctx);
  EXIT_FRAME(ctx->eval_ctx.stack);
  CTX_PUSH(ctx, obj_rval);

  return obj_true;
}

Obj *set(Obj *void_obj, ClsrContext *ctx) {
  (void)void_obj;

  Obj *key = CTX_POP(ctx);
  Obj *val = CTX_POP(ctx);

  if (!OBJ_IS_LITERAL_SYM(key)) {
    assert(0 && "Obj is not a literal symbol.");
    return obj_false; // TODO: error handling
  }

  env_set(CTX_ENV(ctx), OBJ_AS(key, literal).symbol,
          val); // TODO: error handling

  return obj_true;
}

Obj *eval(Obj *obj, ClsrContext *ctx) {
  Obj *result = obj_false;

  if (obj == NULL) {
    return obj_false;
  }

  if (!OBJ_IS_LIST(obj)) {
    assert(0 && "Obj is not a list.");
    return obj_false; // TODO: error handling
  }

  ObjList expressions = OBJ_AS(obj, list);

  for (unsigned int i = 0; i < expressions.count; ++i) {
    Obj *expression = expressions.nodes[i];

    switch (OBJ_KIND(expression)) {
    case Obj_Literal:
      if (OBJ_IS_LITERAL_KEYWRD(expression)) {
        CTX_PUSH(ctx, expression);
        result = expression;
      }
      break;
    case Obj_Call:
      result = OBJ_AS(expression, call).prim->prim_fun(expression, ctx);
      break;
    case Obj_Closure:
      result = closure(expression, ctx);
      break;
    case Obj_If:
      result = if_(expression, ctx);
      break;
    default:
      die("Unknown to eval\n");
      return obj_false;
    }
  }
  return result;
}
