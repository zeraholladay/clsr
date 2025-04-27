#include "clsr.h"

Obj *push(Obj *obj, EvalContext *ctx) {
  (void)ctx; // FIXME

  ObjCall obj_call = OBJ_AS(obj, call);

  ObjList list = OBJ_AS(obj_call.args, list);
  for (unsigned int i; i < list.count; ++i) {
    PUSH(ctx->stack, list.nodes[i]);
  }
  return NULL;
}

Obj *eval_call(ObjCall *obj_call, EvalContext *ctx) {
  // switch (obj_call->prim->op_code) {
  // case PrimOp_Apply:
  //   // handle APPLY operation
  //   break;
  // case PrimOp_Lookup:
  //   // handle LOOKUP operation
  //   break;
  // case PrimOp_Push:
  //   ObjList *list = &OBJ_AS(obj_call->args, list);
  //   for (unsigned int i; i < list->count; ++i) {
  //     PUSH(ctx->stack, list->nodes[i]);
  //   }
  //   break;
  // case PrimOp_Return:
  //   // handle RETURN operation
  //   break;
  // case PrimOp_Set:
  //   Obj *obj_val = POP(ctx->stack);
  //   Obj *obj_key = POP(ctx->stack);
  //   // FIXME: key must be a symbol
  //   ObjLiteral literal = OBJ_AS(obj_key, literal);
  //   env_set(ctx->env, literal.symbol, obj_val);
  //   break;
  // default:
  //   die("Unknown op_code\n");
  //   break;
  // }
  return NULL;
}

Obj *closure(Obj *obj, EvalContext *ctx) { return NULL; }

Obj *eval(Obj *obj, EvalContext *ctx) {
  if (OBJ_KIND(obj) == Obj_Call) {
    return OBJ_AS(obj, call).prim->prim_func(obj, ctx);
  } else if (OBJ_KIND(obj) == Obj_Closure) {
    return closure(obj, ctx);
  } else {
    die("Unknown to eval\n");
    return NULL;
  }
}
