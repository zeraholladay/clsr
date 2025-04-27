#include "eval.h"

#include "env.h"

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

Obj *eval_closure(Obj *call, EvalContext *ctx) { return NULL; }

Obj *eval(Obj *obj, EvalContext *ctx) {
  switch (OBJ_KIND(obj)) {
  case Obj_Call:
    eval_call(&OBJ_AS(obj, call), ctx);
    break;
  case Obj_Closure:
    eval_closure(obj, ctx);
    break;
  default:
    fprintf(stderr, "Unknown object kind: %d\n", obj->kind);
    return NULL;
    break;
  }
  return NULL;
}
