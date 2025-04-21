#include "eval.h"

Obj *eval_call(Obj *obj, EvalContext *ctx) {
  switch (OBJ_AS(obj, call).prim->op_code) {
  case PrimOp_Apply:
    // handle APPLY operation
    break;

  case PrimOp_Lookup:
    // handle LOOKUP operation
    break;

  case PrimOp_Push:
    for () {
    }

    break;

  case PrimOp_Return:
    // handle RETURN operation
    break;

  case PrimOp_Set:
    // handle SET operation
    break;

  default:
    die("Unknown op_code\n");
    break;
  }
  return NULL;
}

Obj *eval_closure(Obj *call, EvalContext *ctx) { return NULL; }

Obj *eval(Obj *obj, EvalContext *ctx) {
  switch (obj->kind) {
  case Obj_Call:
    eval_call(obj, ctx);
    break;
  case Obj_Closure:
    eval_closure(obj, ctx);
    break;

  default:
    // handle unknown or invalid object kind
    fprintf(stderr, "Unknown object kind: %d\n", obj->kind);
    break;
  }
  return NULL;
}
