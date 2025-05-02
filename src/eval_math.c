#include <assert.h>

#include "clsr.h"
#include "stack.h"

Obj *add(Obj *void_obj, ClsrContext *ctx) {
  (void)void_obj;

  Obj *arg1 = CTX_POP(ctx);
  Obj *arg2 = CTX_POP(ctx);

  if (!OBJ_IS_LITERAL_INT(arg1) || !OBJ_IS_LITERAL_INT(arg2)) {
    assert(0 && "Objs must be an integer in add");
    return obj_false; // TODO: error message
  }

  int result = OBJ_AS(arg1, literal).integer + OBJ_AS(arg2, literal).integer;

  Obj *new_obj = obj_new_literal_int(ctx->obj_pool, result);

  PUSH(ctx->eval_ctx.stack, new_obj);

  return obj_true;
}

Obj *sub(Obj *void_obj, ClsrContext *ctx) {
  (void)void_obj;

  Obj *arg1 = CTX_POP(ctx);
  Obj *arg2 = CTX_POP(ctx);

  if (!OBJ_IS_LITERAL_INT(arg1) || !OBJ_IS_LITERAL_INT(arg2)) {
    assert(0 && "Objs must be an integer in sub");
    return obj_false; // TODO: error message
  }

  int result = OBJ_AS(arg1, literal).integer - OBJ_AS(arg2, literal).integer;

  Obj *new_obj = obj_new_literal_int(ctx->obj_pool, result);

  PUSH(ctx->eval_ctx.stack, new_obj);

  return obj_true;
}

Obj *mul(Obj *void_obj, ClsrContext *ctx) {
  (void)void_obj;

  Obj *arg1 = CTX_POP(ctx);
  Obj *arg2 = CTX_POP(ctx);

  if (!OBJ_IS_LITERAL_INT(arg1) || !OBJ_IS_LITERAL_INT(arg2)) {
    assert(0 && "Objs must be an integer in mul");
    return obj_false; // TODO: error message
  }

  int result = OBJ_AS(arg1, literal).integer * OBJ_AS(arg2, literal).integer;

  Obj *new_obj = obj_new_literal_int(ctx->obj_pool, result);

  PUSH(ctx->eval_ctx.stack, new_obj);

  return obj_true;
}

Obj *div_(Obj *void_obj, ClsrContext *ctx) {
  (void)void_obj;

  Obj *arg1 = CTX_POP(ctx);
  Obj *arg2 = CTX_POP(ctx);

  if (!OBJ_IS_LITERAL_INT(arg1) || !OBJ_IS_LITERAL_INT(arg2)) {
    assert(0 && "Objs must be an integer in div");
    return obj_false; // TODO: error message
  }

  if (OBJ_AS(arg2, literal).integer == 0)
    return obj_false;

  int result = OBJ_AS(arg1, literal).integer / OBJ_AS(arg2, literal).integer;

  Obj *new_obj = obj_new_literal_int(ctx->obj_pool, result);

  PUSH(ctx->eval_ctx.stack, new_obj);

  return obj_true;
}
