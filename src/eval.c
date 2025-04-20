#include "eval.h"
#include "ast.h"
#include "common.h"
#include "object.h"

Object *eval_ast(ASTNode *node, EvalContext *ctx) {
  switch (node->type) {
  case AST_Literal:
    return object_from_value(&node->literal); // Push or return

  case AST_List:
    for (size_t i = 0; i < node->list.count; ++i) {
      eval_ast(node->list.items[i], ctx);
    }
    return NULL;

  case AST_Call:
    return eval_call(node, ctx);

  case AST_Closure:
    return make_closure(node, ctx->env); // Wrap params/body/env in object

  default:
    fprintf(stderr, "Unknown AST node type %d\n", node->type);
    exit(EXIT_FAILURE);
  }
}

Object *eval_call(ASTNode *call, EvalContext *ctx) {
  const char *op = call->call.op_name;

  if (strcmp(op, "PUSH") == 0) {
    for (size_t i = 0; i < call->call.args->list.count; ++i) {
      Object *obj = eval_ast(call->call.args->list.items[i], ctx);
      push(ctx->stack, obj);
    }
  } else if (strcmp(op, "LOOKUP") == 0) {
    Object *symbol = pop(ctx->stack);
    Object *value = env_lookup(ctx->env, symbol->as.symbol);
    push(ctx->stack, value);
  } else if (strcmp(op, "SET") == 0) {
    Object *value = pop(ctx->stack);
    Object *symbol = pop(ctx->stack);
    env_set(ctx->env, symbol->as.symbol, value);
  } else if (strcmp(op, "RETURN") == 0) {
    // Optional: Pop result and return it to the caller
  } else if (strcmp(op, "APPLY") == 0) {
    Object *closure = pop(ctx->stack);
    return eval_apply(closure, ctx);
  }

  return NULL;
}
