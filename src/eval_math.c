#include <assert.h>

#include "clsr.h"
#include "stack.h"

int is_integer(Obj *obj) {
  if (!obj || !(OBJ_ISKIND(obj, Obj_Literal)) ||
      !(OBJ_AS(obj, literal).kind == Literal_Int)) {
    return 0;
  }
  return 1;
}

Obj *add(Obj *void_obj, ClsrContext *ctx) {
  (void)void_obj;

  Obj *arg1 = POP(ctx->eval_ctx.stack);
  Obj *arg2 = POP(ctx->eval_ctx.stack);

  assert(arg1);
  assert(OBJ_ISKIND(arg1, Obj_Literal));
  assert(OBJ_AS(arg1, literal).kind == Literal_Int);

  assert(arg2);
  assert(OBJ_ISKIND(arg2, Obj_Literal));
  assert(OBJ_AS(arg2, literal).kind == Literal_Int);

  if (!is_integer(arg1) || !is_integer(arg2)) {
    return obj_false; // TODO: error message
  }

  int result = OBJ_AS(arg1, literal).integer + OBJ_AS(arg2, literal).integer;

  Obj *new_obj = obj_new_literal_int(ctx->obj_pool, result);

  PUSH(ctx->eval_ctx.stack, new_obj);

  return obj_true;
}

Obj *sub(Obj *void_obj, ClsrContext *ctx) {
  (void)void_obj;

  Obj *arg1 = POP(ctx->eval_ctx.stack);
  Obj *arg2 = POP(ctx->eval_ctx.stack);

  assert(arg1);
  assert(OBJ_ISKIND(arg1, Obj_Literal));
  assert(OBJ_AS(arg1, literal).kind == Literal_Int);

  assert(arg2);
  assert(OBJ_ISKIND(arg2, Obj_Literal));
  assert(OBJ_AS(arg2, literal).kind == Literal_Int);

  if (!is_integer(arg1) || !is_integer(arg2)) {
    return obj_false; // TODO: error message
  }

  int result = OBJ_AS(arg1, literal).integer - OBJ_AS(arg2, literal).integer;

  Obj *new_obj = obj_new_literal_int(ctx->obj_pool, result);

  PUSH(ctx->eval_ctx.stack, new_obj);

  return obj_true;
}

Obj *mul(Obj *void_obj, ClsrContext *ctx) {
  (void)void_obj;

  Obj *arg1 = POP(ctx->eval_ctx.stack);
  Obj *arg2 = POP(ctx->eval_ctx.stack);

  assert(arg1);
  assert(OBJ_ISKIND(arg1, Obj_Literal));
  assert(OBJ_AS(arg1, literal).kind == Literal_Int);

  assert(arg2);
  assert(OBJ_ISKIND(arg2, Obj_Literal));
  assert(OBJ_AS(arg2, literal).kind == Literal_Int);

  if (!is_integer(arg1) || !is_integer(arg2)) {
    return obj_false; // TODO: error message
  }

  int result = OBJ_AS(arg1, literal).integer * OBJ_AS(arg2, literal).integer;

  Obj *new_obj = obj_new_literal_int(ctx->obj_pool, result);

  PUSH(ctx->eval_ctx.stack, new_obj);

  return obj_true;
}

Obj *div_(Obj *void_obj, ClsrContext *ctx) {
  (void)void_obj;

  Obj *arg1 = POP(ctx->eval_ctx.stack);
  Obj *arg2 = POP(ctx->eval_ctx.stack);

  assert(arg1);
  assert(OBJ_ISKIND(arg1, Obj_Literal));
  assert(OBJ_AS(arg1, literal).kind == Literal_Int);

  assert(arg2);
  assert(OBJ_ISKIND(arg2, Obj_Literal));
  assert(OBJ_AS(arg2, literal).kind == Literal_Int);

  if (!is_integer(arg1) || !is_integer(arg2)) {
    return obj_false; // TODO: error message
  }

  if (OBJ_AS(arg2, literal).integer == 0)
    return obj_false;

  int result = OBJ_AS(arg1, literal).integer / OBJ_AS(arg2, literal).integer;

  Obj *new_obj = obj_new_literal_int(ctx->obj_pool, result);

  PUSH(ctx->eval_ctx.stack, new_obj);

  return obj_true;
}
