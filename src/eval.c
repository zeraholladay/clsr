// #include "common.h"
// #include "eval.h"
// #include "obj.h"

// Obj *eval_ast(Obj *obj, EvalContext *ctx) {
//   switch (obj->type) {
//   case Obj_Literal:
//     return object_from_value(&obj->literal); // Push or return

//   case Obj_List:
//     for (size_t i = 0; i < obj->list.count; ++i) {
//       eval_ast(obj->list.items[i], ctx);
//     }
//     return NULL;

//   case Obj_Call:
//     return eval_call(obj, ctx);

//   case Obj_Closure:
//     return make_closure(obj, ctx->env); // Wrap params/body/env in object

//   default:
//     fprintf(stderr, "Unknown AST obj type %d\n", obj->type);
//     exit(EXIT_FAILURE);
//   }
// }

// Obj *eval_call(Obj *call, EvalContext *ctx) {
//   const char *op = call->call.op_name;

//   if (strcmp(op, "PUSH") == 0) {
//     for (size_t i = 0; i < call->call.args->list.count; ++i) {
//       Obj *obj = eval_ast(call->call.args->list.items[i], ctx);
//       push(ctx->stack, obj);
//     }
//   } else if (strcmp(op, "LOOKUP") == 0) {
//     Obj *symbol = pop(ctx->stack);
//     Obj *value = env_lookup(ctx->env, symbol->as.symbol);
//     push(ctx->stack, value);
//   } else if (strcmp(op, "SET") == 0) {
//     Obj *value = pop(ctx->stack);
//     Obj *symbol = pop(ctx->stack);
//     env_set(ctx->env, symbol->as.symbol, value);
//   } else if (strcmp(op, "RETURN") == 0) {
//     // Optional: Pop result and return it to the caller
//   } else if (strcmp(op, "APPLY") == 0) {
//     Obj *closure = pop(ctx->stack);
//     return eval_apply(closure, ctx);
//   }

//   return NULL;
// }
